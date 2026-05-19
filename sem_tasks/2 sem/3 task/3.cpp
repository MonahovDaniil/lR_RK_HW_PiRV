#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>

using namespace std;


void inputGrades(vector<vector<double>>& grades, int N, int M)
{
    for (int i = 0; i < N; i++)
    {
        cout << "\nСтудент " << i + 1 << ":\n";
        for (int j = 0; j < M; j++)
        {
            double value;
            while (true)
            {
                cout << "Оценка по предмету " << j + 1 << " в пятибалльной системе:\n";
                cin >> value;

                if (value < 0.0 || value > 5.0)
                {
                    cout << "Ошибка ввода, повторите попытку снова.\n";
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

double findStudentAverage(const vector<double>& studentGrades)
{
    double sum = 0.0;
    for (double grade : studentGrades)
        sum += grade;

    return sum / studentGrades.size();
}

vector<double> findAllStudentsAverage(const vector<vector<double>>& grades)
{
    vector<double> averages;

    for (const auto& student : grades)
        averages.push_back(findStudentAverage(student));

    return averages;
}

vector<double> findSubjectAverages(const vector<vector<double>>& grades, int M)
{
    vector<double> subjectAverages(M, 0.0);

    for (int j = 0; j < M; j++)
    {
        for (size_t i = 0; i < grades.size(); i++)
            subjectAverages[j] += grades[i][j];

        subjectAverages[j] /= grades.size();
    }

    return subjectAverages;
}

int findBestStudent(const vector<double>& averages)
{
    int bestIndex = 0;

    for (size_t i = 1; i < averages.size(); i++)
        if (averages[i] > averages[bestIndex])
            bestIndex = i;

    return bestIndex;
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
        cout << "Ошибка - значения должны быть неотрицательными\n";
        return 1;
    }

    vector<vector<double>> grades(N, vector<double>(M));

    inputGrades(grades, N, M);

    vector<double> studentAverages = findAllStudentsAverage(grades);
    vector<pair<int, double>> students;

    for (size_t i = 0; i < studentAverages.size(); i++)
    {
        students.push_back({static_cast<int>(i), studentAverages[i]});
    }

    double eps = 1e-9;

    sort(students.begin(), students.end(),[eps](const pair<int,double>& a,
        const pair<int,double>& b)
     {
         if (fabs(a.second - b.second) > eps)
             return a.second > b.second;

         return a.first < b.first;
     });

    cout << "\nОтсортированный список студентов:\n";

    for (const auto& student : students)
    {
        cout << "Студент " << student.first + 1
         << " - Средний балл: "
         << student.second << endl;
    }

    vector<double> subjectAverages = findSubjectAverages(grades, M);

    cout << "\nСредние баллы студентов:\n";
    for (size_t i = 0; i < studentAverages.size(); i++)
        cout << "Студент " << i + 1 << ": "
             << studentAverages[i] << endl;

    cout << "\nСредние баллы по предметам:\n";
    for (size_t j = 0; j < subjectAverages.size(); j++)
        cout << "Предмет " << j + 1 << ": "
             << subjectAverages[j] << endl;

    int bestStudent = findBestStudent(studentAverages);
    cout << "\nСтудент с максимальным средним баллом: "
         << bestStudent + 1
         << " ("
         << studentAverages[bestStudent] << ")\n";

    return 0;
}