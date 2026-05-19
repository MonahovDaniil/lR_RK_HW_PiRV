#include <iostream>
#include <vector>

class VirtualThread {
private:
    std::vector<int> tasks;
    int index = 0;

public:
    VirtualThread(std::vector<int> t) : tasks(t) {}

    bool hasTasks() {
        return index < tasks.size();
    }

    void run(int threadId) {

        int n = tasks[index];

        long long factorial = 1;
        for (int i = 1; i <= n; i++)
            factorial *= i;

        std::cout << "Виртуальный поток "
                  << threadId
                  << " вычисляет "
                  << n << "! = "
                  << factorial
                  << std::endl;

        index++;
    }
};