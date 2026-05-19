#include <vector>
#include "Student.hpp"
#include "Teacher.hpp"

int main()
{
    Student* s1 = new Student("Иванов", "22У654");
    Student* s2 = new Student("Петров", "22У655");

    s1->addGrade(4.65);
    s1->addGrade(5.0);

    s2->addGrade(3.87);
    s2->addGrade(3.90);

    Teacher* t1 = new Teacher("Руднев", "Математика");

    std::vector<Person*> people;

    people.push_back(s1);
    people.push_back(s2);
    people.push_back(t1);

    for (const auto& person : people)
        person->print();

    for (auto person : people)
        delete person;

    return 0;
}