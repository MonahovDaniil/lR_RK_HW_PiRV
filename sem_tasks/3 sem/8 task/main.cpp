#include <iostream>
#include <random>
#include "Task.hpp"
#include "VirtualThread.hpp"
#include "HyperThreadingSimulator.hpp"

Task generateRandomTask(std::mt19937& gen) {
    std::uniform_int_distribution<int> valueDist(1,50);
    std::uniform_int_distribution<int> priorityDist(1,10);
    std::uniform_int_distribution<int> durationDist(200,1000);
    std::uniform_int_distribution<int> stepsDist(2,5);

    return Task(
        valueDist(gen),
        priorityDist(gen),
        durationDist(gen),
        stepsDist(gen)
    );
}

int main() {
    std::mt19937 gen(42);

    VirtualThread t1;
    VirtualThread t2;

    for(int i=0;i<3;i++) {
        t1.addTask(generateRandomTask(gen));
        t2.addTask(generateRandomTask(gen));
    }

    HyperThreadingSimulator sim(t1,t2);
    sim.execute();
}