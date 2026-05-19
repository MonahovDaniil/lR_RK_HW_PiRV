#include "RecordBook.hpp"

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

const std::vector<double>& RecordBook::getGrades() const
{
    return grades;
}

std::string RecordBook::getRecordNumber() const
{
    return recordNumber;
}