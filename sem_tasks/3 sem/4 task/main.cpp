#include <iostream>
#include <vector>
#include <random>
#include "TaskTimer.cpp"

int main() {

    const int SIZE = 100000;

    std::mt19937 gen(42);
    std::uniform_int_distribution<int> dist(1, 100000);

    TaskTimer timer;

    for (int j = 0; j < 5; j++) {

        std::vector<int> arr(SIZE);

        for (int i = 0; i < SIZE; i++) {
            arr[i] = dist(gen);
        }

        timer.measureSorting(arr);
    }

    return 0;
}