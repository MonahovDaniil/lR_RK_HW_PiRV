#include <queue>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <iostream>

template<typename T>
struct PriorityItem {
    T value;
    int priority;

    bool operator<(const PriorityItem& other) const {
        return priority < other.priority;
    }
};

template<typename T>
class PriorityQueue {
private:
    std::priority_queue<PriorityItem<T>> queue;
    mutable std::mutex mtx;
    std::condition_variable cv;

public:
    void push(T value, int priority) {
        {
            std::lock_guard<std::mutex> lock(mtx);
            queue.push({value, priority});
            std::cout << "[Поток " << std::this_thread::get_id()
                      << "] Добавлен элемент: " << value
                      << " с приоритетом " << priority << std::endl;
        }
        cv.notify_one();
    }

    T pop() {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [this]() { return !queue.empty(); });

        auto item = queue.top();
        queue.pop();
        std::cout << "[Поток " << std::this_thread::get_id()
                  << "] Извлечен элемент: " << item.value
                  << " с приоритетом " << item.priority << std::endl;
        return item.value;
    }

    size_t size() const {
        std::lock_guard<std::mutex> lock(mtx);
        return queue.size();
    }
};