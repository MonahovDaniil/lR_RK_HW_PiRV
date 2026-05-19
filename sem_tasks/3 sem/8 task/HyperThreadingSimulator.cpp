#include "HyperThreadingSimulator.hpp"

HyperThreadingSimulator::HyperThreadingSimulator(VirtualThread t1, VirtualThread t2)
    : thread1(t1), thread2(t2) {}

void HyperThreadingSimulator::execute() {
    while (thread1.hasTasks() || thread2.hasTasks()) {
        if (thread1.hasTasks())
            thread1.runStep(1);
        if (thread2.hasTasks())
            thread2.runStep(2);
    }
}