#include <iostream>
#include <vector>
#include <fstream>

using namespace std;

#pragma pack(push, 1)
struct FileHeader
{
    char signature[4];  
    int version; 
    int studentCount;
};
#pragma pack(pop)

const char Expected[4] = {'G','R','D','1'};

void inputGrades(vector<vector<double>>& grades)
{
    for (int i = 0; i < grades.size(); i++)
    {
        cout << "\nСтудент " << i + 1 << ":\n";
        for (int j = 0; j < grades[i].size(); j++)
        {
            double value;

            while (true)
            {
                cout << "  Оценка по предмету " << j + 1 << " (0-5): ";
                cin >> value;

                if (value < 0.0 || value > 5.0)
                {
                    cout << "Ошибка ввода. Повторите.\n";
                }
                else
                {
                    grades[i][j] = value;
                    break;
                }
            }
        }
    }
}

void saveToFile(const string& filename,
                const vector<vector<double>>& grades)
{
    ofstream out(filename, ios::binary);

    if (!out)
    {
        cout << "Ошибка открытия файла для записи\n";
        return;
    }

    FileHeader header;
    for (int i = 0; i < 4; i++) {
        header.signature[i] = Expected[i];
    }
    header.version = 1;
    header.studentCount = grades.size();

    cout << "\nРазмер структуры заголовка: "
         << sizeof(FileHeader) << " байт\n";

    out.write(reinterpret_cast<char*>(&header), sizeof(header));

    int subjectCount = grades[0].size();
    out.write(reinterpret_cast<char*>(&subjectCount), sizeof(subjectCount));

    for (const auto& student : grades)
    {
        out.write(reinterpret_cast<const char*>(student.data()),
                  subjectCount * sizeof(double));
    }

    out.close();
    cout << "Данные успешно сохранены в файл\n";
}

void loadFromFile(const string& filename)
{
    ifstream in(filename, ios::binary);

    if (!in)
    {
        cout << "Ошибка открытия файла для чтения\n";
        return;
    }

    FileHeader header;
    in.read(reinterpret_cast<char*>(&header), sizeof(header));

    if (!equal(header.signature,
           header.signature + 4,
           Expected))
    {
        cout << "Ошибка: неверная сигнатура файла\n";
        return;
    }

    int subjectCount;
    in.read(reinterpret_cast<char*>(&subjectCount), sizeof(subjectCount));

    vector<vector<double>> grades(header.studentCount,
                                  vector<double>(subjectCount));

    for (int i = 0; i < header.studentCount; i++)
    {
        in.read(reinterpret_cast<char*>(grades[i].data()),
                subjectCount * sizeof(double));
    }

    cout << "\nДанные успешно загружены из файла\n";
    cout << "Версия файла: " << header.version << endl;
    cout << "Количество студентов: " << header.studentCount << endl;
    cout << "Количество предметов: " << subjectCount << endl;

    cout << "\nОценки:\n";
    for (int i = 0; i < grades.size(); i++)
    {
        cout << "Студент " << i + 1 << ": ";
        for (double g : grades[i])
            cout << g << " ";
        cout << endl;
    }

    in.close();
}

int main()
{
    int N, M;

    cout << "Введите количество студентов: ";
    cin >> N;

    cout << "Введите количество предметов: ";
    cin >> M;

    if (N <= 0 || M <= 0)
    {
        cout << "Ошибка: значения должны быть натуральными\n";
        return 1;
    }

    vector<vector<double>> grades(N, vector<double>(M));

    inputGrades(grades);

    saveToFile("C:\\Users\\PC\\vsprojecct\\1sem\\5 task\\grades.bin", grades);

    loadFromFile("grades.bin");

    return 0;
}