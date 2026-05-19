#include <iostream>
#include <chrono>

long long summ(int N) {
    long long sum = 0;
    for (int i = 1; i <= N; i++) {
        sum += i;
    }
    return sum;
}

int main() {
    int N;
    std::cout << "Введите N: ";
    std::cin >> N;

    // Засекаем время начала
    auto start = std::chrono::high_resolution_clock::now();

    long long result = summ(N);

    // Засекаем время окончания
    auto end = std::chrono::high_resolution_clock::now();

    // Вычисляем разницу
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    std::cout << "Сумма = " << result << std::endl;
    std::cout << "Время выполнения: " << duration.count() << " миллисекунд" << std::endl;

    return 0;
}