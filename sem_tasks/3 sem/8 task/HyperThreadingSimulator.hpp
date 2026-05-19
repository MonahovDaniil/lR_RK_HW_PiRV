#pragma once
#include "VirtualThread.hpp"

class HyperThreadingSimulator {
private:
    VirtualThread thread1;
    VirtualThread thread2;

public:
    HyperThreadingSimulator(VirtualThread t1, VirtualThread t2);
    void execute();
};