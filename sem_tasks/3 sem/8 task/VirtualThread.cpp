#include "VirtualThread.hpp"
#include <iostream>
#include <algorithm>
#include <thread>
#include <chrono>

void VirtualThread::addTask(const Task& task) {
    tasks.push_back(task);
}

bool VirtualThread::hasTasks() {
    return !tasks.empty();
}

void VirtualThread::runStep(int threadId) {
    if (tasks.empty()) return;

    auto it = std::max_element(tasks.begin(), tasks.end(),
        [](const Task& a, const Task& b) {
            return a.priority < b.priority;
        });

    Task& task = *it;
    task.currentStep++;

    std::cout << "Виртуальный поток "
              << threadId
              << " выполняет шаг "
              << task.currentStep << "/"
              << task.steps
              << " задачи "
              << task.value
              << " с приоритетом "
              << task.priority
              << std::endl;

    std::this_thread::sleep_for(std::chrono::milliseconds(task.duration_ms / task.steps));

    if (task.currentStep == task.steps) {
        int result = task.value * task.value;
        std::cout << "Виртуальный поток "
                  << threadId
                  << " завершил задачу "
                  << task.value
                  << ": результат = "
                  << result
                  << std::endl;

        tasks.erase(it);
    }
}