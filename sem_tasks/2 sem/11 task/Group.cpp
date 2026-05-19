#include "Group.hpp"
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

void Group::removeStudent(const std::string& name)
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
    std::cout << "Группа: " << groupName << std::endl;
    for (const auto& s : students)
        s->print();
}

double Group::calculateAverage() const
{
    if (students.empty())
        return 0.0;

    double sum = 0;
    for (const auto& s : students)
        sum += s->calculateAverage();

    return sum / students.size();
}

const std::vector<Student*>& Group::getStudents() const
{
    return students;
}

std::string Group::getName() const
{
    return groupName;
}