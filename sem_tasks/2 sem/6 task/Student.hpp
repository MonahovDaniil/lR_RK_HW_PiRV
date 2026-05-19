#pragma once
#include <vector>
#include <string>
#include <iostream>

class Student
{
private:
    std::string name;
    std::vector<double> grades;

public:
    explicit Student(const std::string& name);

    void addGrade(double grade);
    double calculateAverage() const;
    void print() const;
};