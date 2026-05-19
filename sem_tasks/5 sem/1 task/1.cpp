#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <atomic>
#include <semaphore>
#include <condition_variable>
#include <queue>
#include <chrono>
#include <random>

using namespace std;

struct WaitingThread {
    int priority;
    thread::id id;

    bool operator<(const WaitingThread& other) const {
        return priority < other.priority;
    }
};

template<typename T>
class ResourcePool
{
private:

    vector<T> resources;

    counting_semaphore<> semaphore;
    mutex mtx;
    condition_variable cv;

    atomic<int> failed_attempts{0};

    priority_queue<WaitingThread> wait_queue;

public:

    ResourcePool(vector<T> init_resources)
        : resources(init_resources),
          semaphore(init_resources.size())
    {}

    T acquire(int priority, int timeout_ms)
    {
        thread::id tid = this_thread::get_id();

        if(!semaphore.try_acquire_for(chrono::milliseconds(timeout_ms)))
        {
            failed_attempts++;

            std::lock_guard<std::mutex> lock(mtx);
            cout << "Поток " << tid
                 << " с приоритетом " << priority
                 << " превысил время ожидания\n";

            throw runtime_error("Превышено время ожидания\n");
        }

        unique_lock<mutex> lock(mtx);

        wait_queue.push({priority, tid});

        cv.wait(lock, [&]{
            return wait_queue.top().id == tid;
        });

        T res = resources.back();
        resources.pop_back();

        wait_queue.pop();

        cout << "Поток " << tid
             << " с приоритетом " << priority
             << " получил ресурс\n";

        return res;
    }

    void release(T res, int priority)
    {
        {
            lock_guard<mutex> lock(mtx);

            resources.push_back(res);

            cout << "Поток "
                 << this_thread::get_id()
                 << " с приоритетом " << priority
                 << " освободил ресурс\n";
        }

        semaphore.release();

        cv.notify_all();
    }

    int get_failed_attempts()
    {
        return failed_attempts.load();
    }
};

void worker(ResourcePool<int>& pool, int priority)
{
    try
    {
        int res = pool.acquire(priority, 500);

        this_thread::sleep_for(chrono::milliseconds(300));

        pool.release(res, priority);
    }
    catch(const std::exception& e)
    {
    std::cout << e.what();
    }

    this_thread::yield();
}

int main()
{
    vector<int> resources = {1,2,3};

    ResourcePool<int> pool(resources);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(0,4);

    for(int i=0;i<10;i++)
    {
        thread([&pool,i,&gen,&dist]{
            worker(pool, dist(gen));
        }).detach();
    }

    this_thread::sleep_for(chrono::seconds(5));

    cout << "Неудачные попытки: "
         << pool.get_failed_attempts() << endl;
}