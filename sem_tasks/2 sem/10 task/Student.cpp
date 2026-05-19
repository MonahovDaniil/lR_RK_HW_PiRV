#include "Student.hpp"
#include <iostream>

Student::Student(const std::string& name,
                 const std::string& recordNumber)
    : Person(name),
      recordBook(recordNumber)
{
}

void Student::addGrade(double grade)
{
    recordBook.addGrade(grade);
}

double Student::calculateAverage() const
{
    return recordBook.calculateAverage();
}

void Student::print() const
{
    std::cout << "Студент: " << name << "\n";
    std::cout << "Номер зачётки: "
              << recordBook.getRecordNumber() << "\n";
    std::cout << "Оценки: ";
    recordBook.printGrades();
    std::cout << "\nСредний балл: "
              << calculateAverage()
              << "\n\n";
}