#include <iostream>
#include <vector>

class VirtualThread {

private:
    std::vector<std::string> tasks;
    int index = 0;

public:

    VirtualThread(std::vector<std::string> t) : tasks(t) {}

    bool hasTasks() {
        return index < tasks.size();
    }

    void runNextTask(int threadId) {

        std::cout << "Виртуальный поток "
                  << threadId
                  << " начал "
                  << tasks[index]
                  << std::endl;

        std::cout << "Виртуальный поток "
                  << threadId
                  << " закончил "
                  << tasks[index]
                  << std::endl;

        index++;
    }
};