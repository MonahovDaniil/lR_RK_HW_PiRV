#include "Teacher.hpp"

Teacher::Teacher(const std::string& name,
                 const std::string& subject)
    : Person(name), subject(subject)
{
}

void Teacher::print() const
{
    std::cout << "Преподаватель: "
              << name
              << "\nДисциплина: "
              << subject
              << "\n\n";
}