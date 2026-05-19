#pragma once
#include "Person.hpp"
#include "RecordBook.hpp"

class Student : public Person
{
private:
    RecordBook recordBook;

public:
    Student(const std::string& name,
            const std::string& recordNumber);

    void addGrade(double grade);
    double calculateAverage() const;
    std::string getName() const;

    void print() const override;
};