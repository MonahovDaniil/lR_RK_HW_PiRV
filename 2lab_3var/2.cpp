#include <iostream>
#include <thread>
#include <vector>
#include <queue>
#include <mutex>
#include <random>
#include <chrono>
#include <semaphore>

using namespace std;

struct Car {
    int id;
    bool emergency;
};

struct Intersection {
    int id;

    queue<Car> ns;
    queue<Car> ew;

    mutex mtx;

    int capacity = 15;

    bool emergencyNS = false;
    bool emergencyEW = false;

    counting_semaphore<> sem{5};
};

vector<Intersection> city(10);
mutex cout_mtx;

void process_car(Intersection& inter, Car c, int i, string direction) {

    inter.sem.acquire();

    {
        lock_guard<mutex> out(cout_mtx);
        cout << "Перекресток " << i
             << " пропускает машину " << c.id
             << " (" << direction << ")"
             << (c.emergency ? " (экстренная)" : "") << endl;
    }

    this_thread::sleep_for(chrono::milliseconds(600));

    {
        lock_guard<mutex> out(cout_mtx);
        cout << "Машина " << c.id
             << " проехала перекресток " << i << endl;
    }

    inter.sem.release();
}

void generate_cars() {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> cross(0, 9);
    uniform_int_distribution<> emergency(0, 25);

    int car_id = 0;

    while (true) {
        int i = cross(gen);

        Car c;
        c.id = ++car_id;
        c.emergency = (emergency(gen) == 0);

        {
            lock_guard<mutex> lock(city[i].mtx);
            uniform_int_distribution<> dir(0, 9);

            if (dir(gen) < 7) {
                city[i].ns.push(c);
                if (c.emergency) city[i].emergencyNS = true;
            } else {
                city[i].ew.push(c);
                if (c.emergency) city[i].emergencyEW = true;
            }
        }

        {
            lock_guard<mutex> out(cout_mtx);
            cout << "Машина " << c.id
                 << " прибыла на перекресток " << i
                 << (c.emergency ? " (экстренная)" : "") << endl;
        }

        this_thread::sleep_for(chrono::milliseconds(30));
    }
}

void traffic_light(int i) {
    while (true) {
        Intersection& inter = city[i];

        int ns_size, ew_size;

        {
            lock_guard<mutex> lock(inter.mtx);
            ns_size = inter.ns.size();
            ew_size = inter.ew.size();
        }

        int total = ns_size + ew_size;
        double load = (double)total / inter.capacity;

        int greenTime = 2;
        string activeDirection = "NS";

        if (inter.emergencyNS || inter.emergencyEW) {
            greenTime = 6;
            activeDirection = inter.emergencyNS ? "NS" : "EW";

            lock_guard<mutex> out(cout_mtx);
            cout << "Экстренная ситуация на перекрестке " << i << endl;
        }
        if (abs(ns_size - ew_size) > 3) {
            greenTime = 7;
            activeDirection = (ns_size > ew_size) ? "NS" : "EW";

            lock_guard<mutex> out(cout_mtx);
            cout << "АВАРИЯ на перекрестке " << i << endl;
        }
        if (load > 0.7) {
            greenTime = 5;
            activeDirection = (ns_size > ew_size) ? "NS" : "EW";

            lock_guard<mutex> out(cout_mtx);
            cout << "Перегрузка на перекрестке " << i << endl;
        }
        else {
            static int counter = 0;
            activeDirection = (++counter % 2 == 0) ? "NS" : "EW";
        }

        for (int t = 0; t < greenTime; t++) {
            Car c;
            bool hasCar = false;

            {
                lock_guard<mutex> lock(inter.mtx);

                if (activeDirection == "NS" && !inter.ns.empty()) {
                    c = inter.ns.front();
                    inter.ns.pop();
                    hasCar = true;
                    if (c.emergency) inter.emergencyNS = false;
                }
                else if (activeDirection == "EW" && !inter.ew.empty()) {
                    c = inter.ew.front();
                    inter.ew.pop();
                    hasCar = true;
                    if (c.emergency) inter.emergencyEW = false;
                }
            }

            if (hasCar) {
                thread(process_car, ref(inter), c, i, activeDirection).detach();
            }
        }

        this_thread::sleep_for(chrono::milliseconds(1000));
    }
}

int main() {

    vector<thread> lights;

    for (int i = 0; i < 10; i++) {
        city[i].id = i;
        lights.emplace_back(traffic_light, i);
    }

    thread generator(generate_cars);

    while (true) {
        this_thread::sleep_for(chrono::seconds(1));
    }
    //generator.join();
    // for (auto& t : lights)
    //     t.join();

    return 0;
}