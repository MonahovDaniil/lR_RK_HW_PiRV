#include <iostream>
#include "HyperThreadingSimulator.cpp"

int main() {

    VirtualThread t1({"Задачу A", "Задачу C"});
    VirtualThread t2({"Задачу B", "Задачу D"});

    HyperThreadingSimulator sim(t1, t2);

    sim.execute();
}