#include <iostream>
#include <vector>
#include "VirtualThread.cpp"

class HyperThreadingSimulator {
private:
    VirtualThread thread1;
    VirtualThread thread2;

public:
    HyperThreadingSimulator(VirtualThread t1, VirtualThread t2)
        : thread1(t1), thread2(t2) {}

    void execute() {

        while (thread1.hasTasks() || thread2.hasTasks()) {

            if (thread1.hasTasks())
                thread1.run(1);

            if (thread2.hasTasks())
                thread2.run(2);
        }
    }
};