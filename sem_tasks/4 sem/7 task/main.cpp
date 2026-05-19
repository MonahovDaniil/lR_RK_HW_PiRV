#include "PriorityQueue.cpp"
#include <thread>
#include <chrono>
#include <vector>
#include <cstdlib>
#include <string>

int main() {
    PriorityQueue<std::string> pq;

    auto producer = [&pq](int id) {
        for (int i = 1; i <= 5; ++i) {
            int priority = rand() % 100;
            pq.push("Элемент_" + std::to_string(i) + "_поток_" + std::to_string(id), priority);
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            std::this_thread::yield();
        }
    };

    auto consumer = [&pq]() {
        for (int i = 0; i < 5; ++i) {
            std::string val = pq.pop();
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            std::this_thread::yield();
        }
    };

    std::thread p1(producer, 1);
    std::thread p2(producer, 2);
    std::thread c1(consumer);
    std::thread c2(consumer);

    p1.detach();
    p2.detach();
    c1.detach();
    c2.detach();

    std::this_thread::sleep_for(std::chrono::seconds(10));

    std::cout << "Размер очереди после работы потоков: " << pq.size() << std::endl;

    return 0;
}