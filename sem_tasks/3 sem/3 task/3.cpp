#include <iostream>
#include <chrono>

int main() {
    int totalSeconds;

    std::cout << "Введите количество секунд: ";
    std::cin >> totalSeconds;

    std::chrono::seconds sec(totalSeconds);

    auto hours = std::chrono::duration_cast<std::chrono::hours>(sec);
    sec -= hours;

    auto minutes = std::chrono::duration_cast<std::chrono::minutes>(sec);
    sec -= minutes;

    std::cout << hours.count() << " час "
              << minutes.count() << " минут "
              << sec.count() << " секунд" << std::endl;

    return 0;
}