#include "Teacher.hpp"
#include <iostream>

Teacher::Teacher(const std::string& name,
                 const std::string& subject)
    : Person(name), subject(subject)
{
}

void Teacher::print() const
{
    std::cout << "Преподаватель: "
              << name
              << " | Дисциплина: "
              << subject << std::endl;
}