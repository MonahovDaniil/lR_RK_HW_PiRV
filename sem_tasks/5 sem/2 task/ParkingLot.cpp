#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <semaphore>
#include <condition_variable>
#include <chrono>

using namespace std;

class ParkingLot
{
private:

    int capacity;
    int occupied = 0;

    counting_semaphore<> semaphore;

    mutex mtx;
    condition_variable cv;

    int vip_waiting = 0;

public:

    ParkingLot(int cap)
        : capacity(cap),
          semaphore(cap)
    {}

    void park(bool isVIP, int timeout_ms)
    {
        thread::id tid = this_thread::get_id();

        bool success = semaphore.try_acquire_for(
            chrono::milliseconds(timeout_ms)
        );

        if(!success)
        {
            lock_guard<mutex> lock(mtx);
            if(isVIP)
                vip_waiting--;
            cout << "Поток " << tid
                 << " VIP:" << isVIP
                << " превысил таймаут\n";
            return;
        }

        unique_lock<mutex> lock(mtx);

        if(isVIP)
            vip_waiting++;

        if(!isVIP)
        {
            cv.wait(lock, [&]{
                return vip_waiting == 0;
            });
        }

        if(isVIP)
            vip_waiting--;

        occupied++;

        cout << "Поток " << tid
             << " " << (isVIP ? "VIP" : "Обычный")
            << " припаркован. Занято: "
            << occupied
            << " Свободно: "
            << capacity - occupied
            << endl;

        lock.unlock();

        this_thread::sleep_for(chrono::milliseconds(500));
        leave();
    }

    void leave()
    {
        {
            lock_guard<mutex> lock(mtx);

            occupied--;

            cout << "Поток "
                 << this_thread::get_id()
                 << " уехал. Занято: "
                 << occupied
                 << " Свободно: "
                 << capacity - occupied
                 << endl;
        }

        semaphore.release();

        cv.notify_all();
    }

    void change_capacity(int new_capacity)
    {
        lock_guard<mutex> lock(mtx);

        int diff = new_capacity - capacity;
        capacity = new_capacity;

        if(diff > 0)
        {
            for(int i=0;i<diff;i++)
                semaphore.release();
        }

        cout << "Вместимость парковки изменена на "
             << capacity << endl;
    }
};

void car(ParkingLot& lot, bool vip)
{
    lot.park(vip, 1000);

    this_thread::yield();
}