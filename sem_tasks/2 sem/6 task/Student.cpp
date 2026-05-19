#include "Student.hpp"

Student::Student(const std::string& name)
    : name(name)
{
}

void Student::addGrade(double grade)
{
    if (grade >= 0.0 && grade <= 5.0)
        grades.push_back(grade);
    else
        std::cout << "Неверно указана оценка\n";
}

double Student::calculateAverage() const
{
    if (grades.empty())
        return 0.0;

    double sum = 0.0;

    for (double g : grades)
        sum += g;

    return sum / grades.size();
}

void Student::print() const
{
    std::cout << "Студент: " << name << "\nОценки: ";

    for (double g : grades)
        std::cout << g << " ";

    std::cout << "\nСредний балл: "
              << calculateAverage()
              << "\n\n";
}