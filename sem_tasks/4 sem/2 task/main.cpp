#include <iostream>
#include <thread>
#include <chrono>

#include "Bank.cpp"

int main() {

    Bank<int> bank(5, 1000);

    std::cout << "Начальный общий остаток = " << bank.total_balance() << std::endl;

    for (int i = 0; i < 10; i++) {

        int from = i % 5;
        int to = (i + 1) % 5;

        bank.transfer(from, to, 100);
    }

    std::this_thread::sleep_for(std::chrono::seconds(2));

    std::cout << "Итоговый остаток = " << bank.total_balance() << std::endl;

    return 0;
}