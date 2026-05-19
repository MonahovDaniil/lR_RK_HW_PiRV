#include "Cache.cpp"
#include <thread>
#include <chrono>
#include <string>

int main() {
    Cache<int, std::string> cache;
    auto writer = [&cache]() {
        for (int i = 1; i <= 5; ++i) {
            cache.set(i, std::to_string(i));
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            std::this_thread::yield();
        }
    };

    auto reader = [&cache]() {
        for (int i = 1; i <= 5; ++i) {
            std::string val = cache.get(i);
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            std::this_thread::yield();
        }
    };

    std::thread t1(writer);
    std::thread t2(reader);
    std::thread t3(reader);

    t1.detach();
    t2.detach();
    t3.detach();

    std::this_thread::sleep_for(std::chrono::seconds(2));

    cache.printAll();

    return 0;
}