#include <mutex>
#include <condition_variable>
#include <iostream>

template<typename T>
class Account {

private:
    T balance;
    std::mutex mtx;
    std::condition_variable cv;

public:

    Account(T initial = 0) : balance(initial) {}

    T get_balance() {
        std::lock_guard<std::mutex> lock(mtx);
        return balance;
    }

    void deposit(T amount) {
        std::unique_lock<std::mutex> lock(mtx);

        balance += amount;

        cv.notify_all();
    }

    void withdraw(T amount) {

        std::unique_lock<std::mutex> lock(mtx);

        cv.wait(lock, [&]() {
            return balance >= amount;
        });

        balance -= amount;
    }

    std::mutex& get_mutex() {
        return mtx;
    }

    std::condition_variable& get_cv() {
        return cv;
    }
};