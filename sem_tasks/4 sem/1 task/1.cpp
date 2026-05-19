#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>

template<typename T>
class ParallelSum {

private:
    std::vector<T> data;
    size_t n_threads;

    T total_sum = 0;

    std::mutex mtx;
    std::condition_variable cv;

    int finished_threads = 0;

    inline T sum_segment(size_t start, size_t end) {

        T local_sum = 0;

        for (size_t i = start; i < end; i++) {

            local_sum += data[i];

            if (i % 50 == 0) {
                std::this_thread::yield();
            }
        }

        return local_sum;
    }

public:

    ParallelSum(std::vector<T> d, size_t threads)
        : data(d), n_threads(threads) {}

    T compute_sum() {

        size_t segment = data.size() / n_threads;

        for (size_t t = 0; t < n_threads; t++) {

            size_t start = t * segment;
            size_t end;
            if (t == n_threads - 1) {
                end = data.size();
            } else {
                end = start + segment;
            }

            std::thread([this, start, end]() {

                T local_sum = sum_segment(start, end);

                {
                    std::lock_guard<std::mutex> lock(mtx);

                    total_sum += local_sum;

                    std::cout
                        << "Поток "
                        << std::this_thread::get_id()
                        << " сумма сегмента = "
                        << local_sum
                        << std::endl;

                    finished_threads++;
                }

                cv.notify_one();

            }).detach();
        }

        std::unique_lock<std::mutex> lock(mtx);

        cv.wait(lock, [this]() {
            return finished_threads == n_threads;
        });

        return total_sum;
    }
};

int main() {

    size_t N;
    size_t num_threads;

    std::cout << "Введите количество элементов массива: ";
    std::cin >> N;

    std::cout << "Введите количество потоков: ";
    std::cin >> num_threads;

    std::vector<int> data;

    for (size_t i = 1; i <= N; i++) {
        data.push_back(static_cast<int>(i));
    }

    ParallelSum<int> ps(data, num_threads);

    int result = ps.compute_sum();

    std::cout << "Итоговая сумма = " << result << std::endl;

    return 0;
}