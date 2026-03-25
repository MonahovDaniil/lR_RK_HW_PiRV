#include <iostream>
#include <thread>
#include <queue>
#include <vector>
#include <semaphore>
#include <mutex>
#include <chrono>
#include <atomic>

std::queue<int> queue1;
std::queue<int> queue2;

std::mutex mtx1, mtx2;

std::counting_semaphore<10> sem1(0);
std::counting_semaphore<10> sem2(0);

std::atomic<int> tasksCompleted(0);
std::mutex coutMutex;

void worker(int id) {
    while (tasksCompleted < 10) {
        int task = -1;
        int sourceQueue = 0;
        bool acquired = false;

        if (sem1.try_acquire()) {
            acquired = true;
            std::lock_guard<std::mutex> lock(mtx1);
            if (!queue1.empty()) {
                task = queue1.front();
                queue1.pop();
                sourceQueue = 1;
            }
        }
        else if (sem2.try_acquire()) {
            acquired = true;
            std::lock_guard<std::mutex> lock(mtx2);
            if (!queue2.empty()) {
                task = queue2.front();
                queue2.pop();
                sourceQueue = 2;
            }
        }

        if (!acquired) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }

        if (task != -1) {
            {
                std::lock_guard<std::mutex> lock(coutMutex);
                std::cout << "Worker " << id
                          << " обрабатывает задачу " << task
                          << " из очереди " << sourceQueue << "\n";
                std::cout.flush();
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(500));

            {
                std::lock_guard<std::mutex> lock(coutMutex);
                std::cout << "Worker " << id
                          << " завершил задачу " << task << "\n";
                std::cout.flush();
            }

            tasksCompleted++;
        }
    }

    {
        std::lock_guard<std::mutex> lock(coutMutex);
        std::cout << "Worker " << id << " завершил работу\n";
        std::cout.flush();
    }
}

int main() {
    std::cout << "Запуск обработки задач...\n\n";

    for (int i = 1; i <= 5; i++) {
        queue1.push(i);
        sem1.release();
    }

    for (int i = 6; i <= 10; i++) {
        queue2.push(i);
        sem2.release();
    }

    std::vector<std::thread> workers;

    for (int i = 0; i < 5; i++) {
        workers.emplace_back(worker, i + 1);
    }

    for (auto& t : workers) {
        t.join();
    }

    std::cout << "\nВсе " << tasksCompleted << " задач обработаны, программа завершена.\n";
    return 0;
}