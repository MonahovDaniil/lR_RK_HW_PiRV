#include <iostream>
#include "HyperThreadingSimulator.cpp"

int main() {

    VirtualThread t1({5,10});
    VirtualThread t2({7,12});

    HyperThreadingSimulator sim(t1, t2);

    sim.execute();
}