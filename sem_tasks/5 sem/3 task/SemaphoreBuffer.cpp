#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <semaphore>
#include <chrono>
#include <random>

using namespace std;

template<typename T>
class SemaphoreBuffer
{
private:

    vector<vector<T>> buffers;

    vector<counting_semaphore<>> empty;
    vector<counting_semaphore<>> full;

    vector<mutex> mtx;

    int buffer_size;

public:

    SemaphoreBuffer(int k, int size)
        : buffer_size(size)
    {
        buffers.resize(k);

        for(int i=0;i<k;i++)
        {
            empty.emplace_back(size);
            full.emplace_back(0);
        }

        mtx.resize(k);
    }
        void produce(T value, int buffer_index, int timeout_ms)
    {
        auto tid = this_thread::get_id();

        bool success = empty[buffer_index].try_acquire_for(
            chrono::milliseconds(timeout_ms)
        );

        if(!success)
        {
            lock_guard<mutex> lock(mtx[buffer_index]);

            cout << "Поток " << tid
                 << " буфер " << buffer_index
                 << ": время ожидания на запись истекло\n";

            return;
        }

        {
            lock_guard<mutex> lock(mtx[buffer_index]);

            buffers[buffer_index].push_back(value);

            cout << "Поток " << tid
                 << " произвел " << value
                 << " -> буфер " << buffer_index
                 << endl;
        }

        full[buffer_index].release();
    }
        T consume(int buffer_index, int timeout_ms)
    {
        auto tid = this_thread::get_id();

        bool success = full[buffer_index].try_acquire_for(
            chrono::milliseconds(timeout_ms)
        );

        if(!success)
        {
            lock_guard<mutex> lock(mtx[buffer_index]);

            cout << "Поток " << tid
                 << " буфер " << buffer_index
                 << ": время ожидания на чтение истекло\n";

            return T();
        }

        T value;

        {
            lock_guard<mutex> lock(mtx[buffer_index]);

            value = buffers[buffer_index].back();
            buffers[buffer_index].pop_back();

            cout << "Поток " << tid
                 << " потребил " << value
                 << " <- буфер " << buffer_index
                 << endl;
        }

        empty[buffer_index].release();

        return value;
    }
};

void producer(SemaphoreBuffer<int>& buf)
{
    mt19937 gen(42);
    uniform_int_distribution<> buffer_dist(0,2);
    uniform_int_distribution<> value_dist(1,100);

    int buffer = buffer_dist(gen);
    int value = value_dist(gen);

    buf.produce(value, buffer, 500);

    this_thread::yield();
}

void consumer(SemaphoreBuffer<int>& buf)
{
    mt19937 gen(84);
    uniform_int_distribution<> buffer_dist(0,2);

    int buffer = buffer_dist(gen);

    buf.consume(buffer, 500);

    this_thread::yield();
}