#pragma once
#include <vector>
#include <string>

class RecordBook
{
private:
    std::string recordNumber;
    std::vector<double> grades;

public:
    explicit RecordBook(const std::string& recordNumber);

    void addGrade(double grade);
    double calculateAverage() const;

    const std::vector<double>& getGrades() const;
    std::string getRecordNumber() const;
};