#include "RecordBook.hpp"
#include <iostream>

RecordBook::RecordBook(const std::string& recordNumber)
    : recordNumber(recordNumber)
{
}

void RecordBook::addGrade(double grade)
{
    if (grade >= 0.0 && grade <= 5.0)
        grades.push_back(grade);
}

double RecordBook::calculateAverage() const
{
    if (grades.empty())
        return 0.0;

    double sum = 0.0;
    for (double g : grades)
        sum += g;

    return sum / grades.size();
}

void RecordBook::printGrades() const
{
    for (double g : grades)
        std::cout << g << " ";
}

std::string RecordBook::getRecordNumber() const
{
    return recordNumber;
}