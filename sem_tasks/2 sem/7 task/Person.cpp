#include "Person.hpp"

Person::Person(const std::string& name)
    : name(name)
{
}

Person::~Person()
{
}

void Person::print() const
{
    std::cout << "Имя: " << name << std::endl;
}