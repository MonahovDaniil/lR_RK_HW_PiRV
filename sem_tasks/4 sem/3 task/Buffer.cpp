#include <vector>
#include <mutex>
#include <condition_variable>
#include <iostream>
#include <thread>

template<typename T>
class Buffer {

private:
    std::vector<T> buffer;
    size_t capacity;

    std::mutex mtx;
    std::condition_variable cv;

public:

    Buffer(size_t cap) : capacity(cap) {}

    void produce(T value) {

        std::unique_lock<std::mutex> lock(mtx);

        while (buffer.size() >= capacity) {
            std::this_thread::yield();
            cv.wait(lock);
        }

        buffer.push_back(value);

        std::cout << "Произведено: "
                  << value
                  << " потоком "
                  << std::this_thread::get_id()
                  << std::endl;

        cv.notify_one();
    }

    T consume() {

        std::unique_lock<std::mutex> lock(mtx);

        while (buffer.empty()) {
            std::this_thread::yield();
            cv.wait(lock);
        }

        T value = buffer.front();
        buffer.erase(buffer.begin());

        std::cout << "Потреблено: "
                  << value
                  << " потоком "
                  << std::this_thread::get_id()
                  << std::endl;

        cv.notify_one();

        return value;
    }
};