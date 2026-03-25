#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <vector>

std::mutex mtx;
std::condition_variable cv;

int currentStage = 0;

std::atomic<bool> stagesDone[4];

void stage(int id) {
    std::unique_lock<std::mutex> lock(mtx);

    cv.wait(lock, [id]() {
        return currentStage == id;
    });

    std::cout << "Этап " << id + 1 << " начал работу\n";

    lock.unlock();

    std::this_thread::sleep_for(std::chrono::seconds(1));

    lock.lock();

    std::cout << "Этап " << id + 1 << " завершен\n";

    stagesDone[id].store(true);

    currentStage++;

    lock.unlock();
    cv.notify_all();
}

int main() {
    for (int i = 0; i < 4; i++) {
        stagesDone[i] = false;
    }

    std::vector<std::thread> threads;

    for (int i = 0; i < 4; i++) {
        threads.emplace_back(stage, i);
    }

    for (auto& t : threads) {
        t.join();
    }

    std::cout << "Все этапы завершены!\n";

    return 0;
}