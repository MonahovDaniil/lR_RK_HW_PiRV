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
    
    inline std::string getName() const
    {
        return name;
    }
};