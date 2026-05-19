#pragma once
#include <vector>
#include <string>

class Student;

class Group
{
private:
    std::string groupName;
    std::vector<Student*> students;

public:
    explicit Group(const std::string& name);

    void addStudent(Student* student);
    void removeStudentByName(const std::string& name);

    void printAll() const;
    double calculateGroupAverage() const;

    void sortByAverage();
    void filterByThreshold(double threshold);
};