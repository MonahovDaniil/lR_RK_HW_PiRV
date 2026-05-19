#include <iostream>
#include <vector>
#include <numeric>
#include <algorithm>
#include <execution>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <fstream>
#include <string>
#include <atomic>
#include <barrier>

struct DataChunk {
    int chunk_id;
    std::vector<double> raw_data;
};

struct ProcessingResult {
    int chunk_id;
    double aggregate_value;
    std::vector<double> sorted_data;
    std::vector<double> scan_metrics;
};

class Scheduler {
private:
    std::queue<DataChunk> input_queue;
    std::mutex mtx;
    std::condition_variable cv;
    std::atomic<bool> logic_finished{false};

public:
    void push_data(DataChunk chunk) {
        {
            std::lock_guard<std::mutex> lock(mtx);
            input_queue.push(std::move(chunk));
        }
        cv.notify_one();
    }

    bool pop_data(DataChunk& chunk) {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [this]() { return !input_queue.empty() || logic_finished.load(); });
        
        if (input_queue.empty() && logic_finished.load()) {
            return false;
        }
        
        chunk = std::move(input_queue.front());
        input_queue.pop();
        return true;
    }

    void shutdown() {
        logic_finished.store(true);
        cv.notify_all();
    }
};

class ResultAggregator {
private:
    std::mutex agg_mtx;
    std::atomic<double> global_sum{0.0};
    std::vector<ProcessingResult> final_results;

public:
    void add_result(ProcessingResult res) {
        double old_val = global_sum.load();
        double new_val;
        do {
            old_val = global_sum.load();
            new_val = old_val + res.aggregate_value;
        } while (!global_sum.compare_exchange_weak(old_val, new_val));

        std::lock_guard<std::mutex> lock(agg_mtx);
        final_results.push_back(std::move(res));
    }

    void print_summary() {
        std::lock_guard<std::mutex> lock(agg_mtx);
        std::cout << "\n[Aggregator] Total sum: " << global_sum.load() << std::endl;
        std::cout << "[Aggregator] Chunks processed: " << final_results.size() << std::endl;
    }
};

class StorageVisualizer {
private:
    std::queue<std::string> file_queue;
    std::mutex storage_mtx;
    std::condition_variable storage_cv;
    std::thread logger_thread;
    std::atomic<bool> stop_requested{false};

    void async_write_loop() {
        std::ofstream out_file("pipeline_output.txt");
        while (true) {
            std::string log_item;
            {
                std::unique_lock<std::mutex> lock(storage_mtx);
                storage_cv.wait(lock, [this]() { 
                    return !file_queue.empty() || stop_requested.load(); 
                });
                
                if (file_queue.empty() && stop_requested.load()) break;
                
                log_item = std::move(file_queue.front());
                file_queue.pop();
            }
            out_file << log_item << "\n";
            std::cout << "[Storage]: " << log_item << std::endl;
        }
        out_file.close();
    }

public:
    StorageVisualizer() {
        logger_thread = std::thread(&StorageVisualizer::async_write_loop, this);
    }

    ~StorageVisualizer() {
        stop_requested.store(true);
        storage_cv.notify_one();
        if (logger_thread.joinable()) {
            logger_thread.join();
        }
    }

    void queue_log(const std::string& message) {
        {
            std::lock_guard<std::mutex> lock(storage_mtx);
            file_queue.push(message);
        }
        storage_cv.notify_one();
    }
};

thread_local std::vector<double> tl_processing_buffer;
thread_local std::vector<double> tl_scan_buffer;
thread_local std::atomic<int> tl_local_counter{0};

void worker_thread_routine(int thread_id, Scheduler* scheduler, ResultAggregator* aggregator, 
                           StorageVisualizer* storage, std::barrier<>& sync_barrier) {
    DataChunk chunk;
    tl_local_counter = 0;

    while (scheduler->pop_data(chunk)) {
        tl_processing_buffer = chunk.raw_data;
        tl_scan_buffer.resize(tl_processing_buffer.size());

        sync_barrier.arrive_and_wait();

        double aggregate = std::transform_reduce(
            std::execution::par_unseq,
            tl_processing_buffer.begin(), tl_processing_buffer.end(),
            0.0,
            std::plus<>(),
            [](double val) { return val * val; }
        );

        std::sort(std::execution::par, tl_processing_buffer.begin(), tl_processing_buffer.end());

        std::inclusive_scan(std::execution::par, tl_processing_buffer.begin(), 
                            tl_processing_buffer.end(), tl_scan_buffer.begin());

        tl_local_counter++;

        ProcessingResult result;
        result.chunk_id = chunk.chunk_id;
        result.aggregate_value = aggregate;
        result.sorted_data = tl_processing_buffer;
        result.scan_metrics = tl_scan_buffer;

        aggregator->add_result(std::move(result));

        sync_barrier.arrive_and_wait();

        std::string log_msg = "Thread " + std::to_string(thread_id) + 
                              " | Chunk #" + std::to_string(chunk.chunk_id) + 
                              " | Aggregate: " + std::to_string(aggregate) +
                              " | Local counter: " + std::to_string(tl_local_counter.load());
        storage->queue_log(log_msg);
    }
}

int main() {
    const int num_threads = 4;
    Scheduler scheduler;
    ResultAggregator aggregator;
    StorageVisualizer storage;
    
    std::barrier sync_barrier(num_threads);

    for (int i = 1; i <= 8; ++i) {
        DataChunk chunk;
        chunk.chunk_id = i;
        chunk.raw_data = {4.0, 1.0, 3.0, 2.0, 5.0 * static_cast<double>(i)};
        scheduler.push_data(std::move(chunk));
    }
    scheduler.shutdown();

    std::vector<std::thread> thread_pool;
    for (int i = 0; i < num_threads; ++i) {
        thread_pool.emplace_back(worker_thread_routine, i, &scheduler, 
                                 &aggregator, &storage, std::ref(sync_barrier));
    }

    for (auto& t : thread_pool) {
        if (t.joinable()) t.join();
    }

    aggregator.print_summary();

    return 0;
}