#include <iostream>
#include <thread>
#include <random>
#include <chrono>

#include "TaskScheduler.cpp"

int main()
{
    const int RESOURCE_LIMIT = 4;
    const int WORKERS = 3;
    const int TASK_COUNT = 10;

    TaskScheduler scheduler(RESOURCE_LIMIT);

    for(int i=0;i<WORKERS;i++)
    {
        thread([&scheduler]{
            scheduler.worker();
        }).detach();
    }

    mt19937 gen(42);

    uniform_int_distribution<int> slots_dist(1,2);
    uniform_int_distribution<int> duration_dist(200,800);
    uniform_int_distribution<int> priority_dist(1,5);

    for(int i=0;i<TASK_COUNT;i++)
    {
        Task t;

        t.id = i;
        t.required_slots = slots_dist(gen);
        t.duration_ms = duration_dist(gen);
        t.priority = priority_dist(gen);

        scheduler.submit(t);

        this_thread::sleep_for(chrono::milliseconds(100));
    }

    this_thread::sleep_for(chrono::seconds(8));

    cout << "\nЗавершенные задачи: "
         << scheduler.finished()
         << endl;

    cout << "Среднее время ожидания: "
         << scheduler.average_wait_time()
         << " мс\n";
}