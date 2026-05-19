#include <iostream>
#include <thread>
#include <chrono>
#include <random>

#include "ParkingLot.cpp"

int main()
{
    ParkingLot lot(3);

    for(int i=0;i<10;i++)
    {
        thread([&lot,i]
        {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> dist(0,1);

            bool vip = dist(gen);
            car(lot, vip);
        }).detach();
    }

    this_thread::sleep_for(chrono::seconds(2));

    lot.change_capacity(5);

    this_thread::sleep_for(chrono::seconds(5));
}