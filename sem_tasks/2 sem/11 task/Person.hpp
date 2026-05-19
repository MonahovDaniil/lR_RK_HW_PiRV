#pragma once
#include <string>

class Person
{
protected:
    std::string name;

public:
    explicit Person(const std::string& name);
    virtual ~Person();

    virtual void print() const;
    std::string getName() const;
};