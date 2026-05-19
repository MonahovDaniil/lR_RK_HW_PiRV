#pragma once

class Task {
public:
    int value;
    int priority;
    int duration_ms;
    int steps;
    int currentStep = 0;

    Task(int v, int p, int d, int s);
};