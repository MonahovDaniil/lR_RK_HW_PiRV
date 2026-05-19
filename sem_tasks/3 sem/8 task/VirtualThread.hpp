#pragma once
#include <vector>
#include "Task.hpp"

class VirtualThread {
private:
    std::vector<Task> tasks;

public:
    void addTask(const Task& task);
    bool hasTasks();
    void runStep(int threadId);
};