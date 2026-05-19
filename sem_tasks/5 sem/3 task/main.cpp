#include <iostream>
#include <thread>
#include <chrono>

#include "SemaphoreBuffer.cpp"

int main()
{
    SemaphoreBuffer<int> buffer(3,5);

    for(int i=0;i<10;i++)
    {
        thread([&buffer]{
            producer(buffer);
        }).detach();
    }

    for(int i=0;i<10;i++)
    {
        thread([&buffer]{
            consumer(buffer);
        }).detach();
    }

    this_thread::sleep_for(chrono::seconds(5));
}