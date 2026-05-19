#include <iostream>
#include <vector>
#include <chrono>
#include <algorithm>

class TaskTimer {
private:
    std::chrono::high_resolution_clock::time_point startTime;
    std::chrono::high_resolution_clock::time_point endTime;

public:
    void start() {
        startTime = std::chrono::high_resolution_clock::now();
    }

    void stop() {
        endTime = std::chrono::high_resolution_clock::now();
    }

    long long getDuration() {
        return std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
    }

    void measureSorting(std::vector<int>& arr) {
        start();

        std::sort(arr.begin(), arr.end());

        stop();

        std::cout << "Время выполнения сортировки: "
                  << getDuration()
                  << " миллисекунд" << std::endl;
    }
};