#include <iostream>

using namespace std;


void inputGrades(double* arr, int size)
{
    for (int i = 0; i < size; i++)
    {
        double value;
        while (true)
        {
            cout << "Введите средний балл студента " << i + 1 << " (0-5): ";
            cin >> value;

            if (value < 0.0 || value > 5.0)
            {
                cout << "Ошибка ввода. Повторите.\n";
            }
            else
            {
                arr[i] = value;
                break;
            }
        }
    }
}

double findAverage(const double* arr, int size)
{
    double sum = 0.0;
    for (int i = 0; i < size; i++)
        sum += arr[i];

    return sum / size;
}

double findMax(const double* arr, int size)
{
    double max = arr[0];
    for (int i = 1; i < size; i++)
        if (arr[i] > max)
            max = arr[i];

    return max;
}

double findMin(const double* arr, int size)
{
    double min = arr[0];
    for (int i = 1; i < size; i++)
        if (arr[i] < min)
            min = arr[i];

    return min;
}

int aboveThreshold(const double* arr, int size, double threshold)
{
    int count = 0;
    for (int i = 0; i < size; i++)
        if (arr[i] > threshold)
            count++;
    return count;
}

int main()
{
    int N;

    cout << "Введите количество студентов: ";
    cin >> N;

    if (N <= 0)
    {
        cout << "Ошибка: количество студентов должно быть неотрицательным\n";
        return 1;
    }

    double* grades = new double[N];

    inputGrades(grades, N);

    cout << "\nСредний балл группы: " 
         << findAverage(grades, N) << endl;

    cout << "Максимальный балл: " 
         << findMax(grades, N) << endl;

    cout << "Минимальный балл: " 
         << findMin(grades, N) << endl;

    double threshold;
    cout << "Введите порог: ";
    cin >> threshold;

    cout << "Количество студентов выше порога: "
         << aboveThreshold(grades, N, threshold) << endl;

    delete[] grades;

    return 0;
}