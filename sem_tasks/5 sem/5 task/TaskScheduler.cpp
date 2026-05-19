#include <iostream>
#include <thread>
#include <vector>
#include <queue>
#include <mutex>
#include <atomic>
#include <semaphore>
#include <chrono>
#include <random>

using namespace std;

struct Task
{
    int id;
    int required_slots;
    int duration_ms;
    int priority;

    chrono::steady_clock::time_point submit_time;

    bool operator<(const Task& other) const
    {
        return priority < other.priority;
    }

    void execute()
    {
        this_thread::sleep_for(chrono::milliseconds(duration_ms));
    }
};

class TaskScheduler
{
private:

    priority_queue<Task> tasks;

    counting_semaphore<100> resource_semaphore;

    mutex queue_mutex;

    atomic<int> completed_tasks{0};

    atomic<long long> total_wait_time{0};

    int resource_limit;

public:

    TaskScheduler(int resources)
        : resource_semaphore(resources), resource_limit(resources)
    {}

    void submit(Task task)
    {
        task.submit_time = chrono::steady_clock::now();

        lock_guard<mutex> lock(queue_mutex);

        tasks.push(task);

        cout << "Задача " << task.id
             << " отправлена (приоритет "
             << task.priority
             << ", ресурсы "
             << task.required_slots
             << ")\n";
    }

    inline void execute_task(Task& task)
    {
        task.execute();
    }

    void worker()
    {
        while(true)
        {
            Task task;
            bool has_task = false;

            {
                lock_guard<mutex> lock(queue_mutex);

                if(!tasks.empty())
                {
                    task = tasks.top();
                    tasks.pop();
                    has_task = true;
                }
            }

            if(!has_task)
            {
                this_thread::yield();
                continue;
            }

            auto start_exec = chrono::steady_clock::now();

            long long wait =
                chrono::duration_cast<chrono::milliseconds>(
                    start_exec - task.submit_time
                ).count();

            total_wait_time += wait;

            for(int i=0;i<task.required_slots;i++)
                resource_semaphore.acquire();

            cout << "Поток "
                 << this_thread::get_id()
                 << " начал задачу "
                 << task.id
                 << " (ресурсы "
                 << task.required_slots
                 << ")\n";

            execute_task(task);

            cout << "Поток "
                 << this_thread::get_id()
                 << " завершил задачу "
                 << task.id
                 << endl;

            for(int i=0;i<task.required_slots;i++)
                resource_semaphore.release();

            completed_tasks++;

            this_thread::yield();
        }
    }

    double average_wait_time()
    {
        int done = completed_tasks.load();

        if(done == 0)
            return 0;

        return (double)total_wait_time / done;
    }

    int finished()
    {
        return completed_tasks.load();
    }
};