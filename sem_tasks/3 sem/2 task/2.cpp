#include <iostream>
#include <chrono>
#include <thread>

int main() {
    int N;

    std::cout << "Введите количество секунд: ";
    std::cin >> N;

    while (N > 0) {
        std::cout << "Осталось: " << N << " секунд" << std::endl;

        std::this_thread::sleep_for(std::chrono::seconds(1));

        N--;
    }

    std::cout << "Время вышло!" << std::endl;

    return 0;
}