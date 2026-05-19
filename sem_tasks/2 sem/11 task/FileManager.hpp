#pragma once
#include <string>
#include "Group.hpp"

class FileManager
{
public:
    static void save(const Group& group,
                     const std::string& filename);

    static void load(Group& group,
                     const std::string& filename);
};