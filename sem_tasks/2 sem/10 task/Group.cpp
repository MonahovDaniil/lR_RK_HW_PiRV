#include "Group.hpp"
#include "Student.hpp"
#include <algorithm>
#include <iostream>

Group::Group(const std::string& name)
    : groupName(name)
{
}

void Group::addStudent(Student* student)
{
    students.push_back(student);
}

void Group::removeStudentByName(const std::string& name)
{
    students.erase(
        std::remove_if(students.begin(), students.end(),
            [&name](Student* s)
            {
                return s->getName() == name;
            }),
        students.end());
}

void Group::printAll() const
{
    std::cout << "Группа: " << groupName << "\n\n";

    for (const auto& student : students)
        student->print();
}

double Group::calculateGroupAverage() const
{
    if (students.empty())
        return 0.0;

    double sum = 0.0;

    for (const auto& student : students)
        sum += student->calculateAverage();

    return sum / students.size();
}

void Group::sortByAverage()
{
    std::sort(students.begin(), students.end(),
        [](Student* a, Student* b)
        {
            return a->calculateAverage() >
                   b->calculateAverage();
        });
}

void Group::filterByThreshold(double threshold)
{
    students.erase(
        std::remove_if(students.begin(), students.end(),
            [threshold](Student* s)
            {
                return s->calculateAverage() < threshold;
            }),
        students.end());
}