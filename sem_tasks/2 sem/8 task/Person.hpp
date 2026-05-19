#pragma once
#include <string>
#include <iostream>

class Person
{
protected:
    std::string name;

public:
    explicit Person(const std::string& name);
    virtual ~Person() = default;

    virtual void print() const;
};