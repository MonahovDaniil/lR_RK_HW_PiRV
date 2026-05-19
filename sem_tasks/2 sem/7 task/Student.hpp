#pragma once
#include "Person.hpp"
#include <vector>

class Student : public Person
{
private:
    std::vector<double> grades;

public:
    explicit Student(const std::string& name);

    void addGrade(double grade);
    double calculateAverage() const;

    void print() const override;
};