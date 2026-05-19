#include "FileManager.hpp"
#include <fstream>
#include <cstring>

#pragma pack(push,1)
struct Header
{
    char signature[4];
    int studentCount;
};
#pragma pack(pop)

void FileManager::save(const Group& group,
                       const std::string& filename)
{
    std::ofstream out(filename, std::ios::binary);

    Header header = { {'A','B','C','1'},
                      static_cast<int>(group.getStudents().size()) };

    out.write(reinterpret_cast<char*>(&header),
              sizeof(header));

    for (const auto& s : group.getStudents())
    {
        std::string name = s->getName();
        const RecordBook& rb = s->getRecordBook();

        size_t nameSize = name.size();
        size_t recordSize = rb.getRecordNumber().size();
        size_t gradeCount = rb.getGrades().size();

        out.write(reinterpret_cast<char*>(&nameSize), sizeof(nameSize));
        out.write(name.c_str(), nameSize);

        out.write(reinterpret_cast<char*>(&recordSize), sizeof(recordSize));
        out.write(rb.getRecordNumber().c_str(), recordSize);

        out.write(reinterpret_cast<char*>(&gradeCount), sizeof(gradeCount));
        out.write(reinterpret_cast<const char*>(rb.getGrades().data()),
                  gradeCount * sizeof(double));
    }

    out.close();
}