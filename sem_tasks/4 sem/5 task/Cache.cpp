#pragma once
#include <map>
#include <mutex>
#include <condition_variable>
#include <iostream>
#include <thread>

template<typename Key, typename Value>
class Cache {
private:
    std::map<Key, Value> data;
    mutable std::mutex mtx;
    std::condition_variable cv;

public:
    inline void set(const Key& key, const Value& value) {
        {
            std::lock_guard<std::mutex> lock(mtx);
            data[key] = value;
            std::cout << "[Поток " << std::this_thread::get_id() 
                      << "] Установлен ключ: " << key 
                      << ", значение: " << value << std::endl;
        }
        cv.notify_all();
    }

    inline Value get(const Key& key) {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [&]{ return data.find(key) != data.end(); });

        Value value = data[key];
        std::cout << "[Поток " << std::this_thread::get_id() 
                  << "] Получен ключ: " << key 
                  << ", значение: " << value << std::endl;
        return value;
    }

    void printAll() const {
        std::lock_guard<std::mutex> lock(mtx);
        std::cout << "Содержимое кэша:" << std::endl;
        for (const auto& [k, v] : data) {
            std::cout << "Ключ: " << k << ", Значение: " << v << std::endl;
        }
    }
};