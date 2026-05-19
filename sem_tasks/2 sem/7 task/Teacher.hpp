#pragma once
#include "Person.hpp"

class Teacher : public Person
{
private:
    std::string subject;

public:
    Teacher(const std::string& name,
            const std::string& subject);

    void print() const override;
};