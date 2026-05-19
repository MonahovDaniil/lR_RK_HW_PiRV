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
    std::cout << "Студент: " << name
              << " | Средний балл: "
              << calculateAverage()
              << std::endl;
}

const RecordBook& Student::getRecordBook() const
{
    return recordBook;
}