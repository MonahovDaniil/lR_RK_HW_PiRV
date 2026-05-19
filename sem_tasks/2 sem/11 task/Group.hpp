#pragma once
#include <vector>
#include <string>
#include "Student.hpp"

class Group
{
private:
    std::string groupName;
    std::vector<Student*> students;

public:
    explicit Group(const std::string& name);

    void addStudent(Student* student);
    void removeStudent(const std::string& name);

    void printAll() const;
    double calculateAverage() const;

    const std::vector<Student*>& getStudents() const;
    std::string getName() const;
};