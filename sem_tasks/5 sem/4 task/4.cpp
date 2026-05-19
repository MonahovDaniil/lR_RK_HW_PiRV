#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <semaphore>
#include <chrono>
#include <queue>
#include <random>

using namespace std;

struct Job
{
    string doc;
    int priority;

    bool operator<(const Job& other) const
    {
        return priority < other.priority;
    }
};

class PrinterQueue
{
private:

    int n_printers;

    counting_semaphore<100> semaphore;

    mutex mtx;

    priority_queue<Job> jobs;

public:

    PrinterQueue(int n) : n_printers(n), semaphore(n) {}

    void printJob(string doc, int priority, int timeout_ms)
    {
        auto tid = this_thread::get_id();

        Job job{doc, priority};

        {
            lock_guard<mutex> lock(mtx);
            jobs.push(job);
        }

        bool success = semaphore.try_acquire_for(
            chrono::milliseconds(timeout_ms)
        );

        if(!success)
        {
            lock_guard<mutex> lock(mtx);

            cout << "Поток " << tid
                 << " задание " << doc
                 << " приоритет " << priority
                 << " ПРОСРОЧЕНО (возвращено в очередь)\n";

            jobs.push(job);

            return;
        }

        Job current;

        {
            lock_guard<mutex> lock(mtx);

            if(jobs.empty())
            {
                semaphore.release();
                return;
            }

            current = jobs.top();
            jobs.pop();

            cout << "Поток " << tid
                 << " печатает "
                 << current.doc
                 << " приоритет "
                 << current.priority
                 << endl;
        }

        this_thread::sleep_for(chrono::milliseconds(500));

        thread_local mt19937 gen(random_device{}());
        uniform_int_distribution<int> interrupt_dist(0, 4);
        bool interrupted = interrupt_dist(gen) == 0;

        if(interrupted)
        {
            lock_guard<mutex> lock(mtx);

            cout << "Поток " << tid
                 << " задание " << current.doc
                 << " ПРЕРВАНО\n";

            jobs.push(current);
        }
        else
        {
            lock_guard<mutex> lock(mtx);

            cout << "Поток " << tid
                 << " завершил "
                 << current.doc
                 << endl;
        }

        semaphore.release();

        this_thread::yield();
    }
};

void worker(PrinterQueue& queue, int id)
{
    thread_local mt19937 gen(42 + id);

    uniform_int_distribution<int> priority_dist(1,5);

    int priority = priority_dist(gen);

    string doc = "doc_" + to_string(id);

    queue.printJob(doc, priority, 500);
}

int main()
{
    PrinterQueue queue(2);

    for(int i=0;i<10;i++)
    {
        thread([&queue,i]{
            worker(queue,i);
        }).detach();
    }

    this_thread::sleep_for(chrono::seconds(5));
}