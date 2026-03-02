#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <boost/thread.hpp>
#include <atomic>
#include <mutex>

enum Mode {
    WITHOUT_SYNCHRONIZATION = 1,
    ATOMIC = 2,
    MUTEX = 3
};

int balance_no_sync = 0;
std::atomic<int> balance_atomic(0);
int balance_mutex = 0;
std::mutex mtx;

std::vector<int> allTransactions;

void clientTask(int start, int end, Mode mode)
{
    for (int i = start; i < end; ++i)
    {
        int amount = allTransactions[i];

        switch (mode)
        {
            case WITHOUT_SYNCHRONIZATION:
                balance_no_sync += amount;
                break;

            case ATOMIC:
                balance_atomic += amount;
                break;

            case MUTEX:
            {
                std::lock_guard<std::mutex> lock(mtx);
                balance_mutex += amount;
                break;
            }
        }
    }
}

void runTest(int threadCount, int operations, Mode mode, long long expectedBalance)
{

    std::vector<boost::thread> threads;

    int totalOps = threadCount * operations;
    int blockSize = totalOps / threadCount;

    auto startTime = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < threadCount; ++i)
{
    int start = i * blockSize;
    int end;

    if (i == threadCount - 1)
    {
        end = totalOps;
    }
    else
    {
        end = start + blockSize;
    }

    threads.emplace_back(clientTask, start, end, mode);
}

    for (auto& t : threads)
        t.join();

    auto endTime = std::chrono::high_resolution_clock::now();

    double time =
        std::chrono::duration<double>(endTime - startTime).count();

    long long result = 0;

    switch (mode)
    {
        case WITHOUT_SYNCHRONIZATION:
            result = balance_no_sync;
            std::cout << "Без синхронизации\n";
            break;

        case ATOMIC:
            result = balance_atomic;
            std::cout << "С использованием std::atomic<int>.\n";
            break;

        case MUTEX:
            result = balance_mutex;
            std::cout << "С использованием std::mutex\n";
            break;
    }

    std::cout << "Время: " << time << " секунд\n";
    std::cout << "Итоговый баланс: " << result << "\n";

    if (result == expectedBalance)
        std::cout << "Результат корректен\n\n";
    else
        std::cout << "Результат некорректен (ожидается " << expectedBalance << ")\n\n";
}

int main()
{
    int threadCount;
    int operations;

    std::cout << "Введите количество клиентов (потоков): ";
    std::cin >> threadCount;

    std::cout << "Введите количество транзакций на клиента: ";
    std::cin >> operations;

    int totalOps = threadCount * operations;

    std::mt19937 gen(42);
    std::uniform_int_distribution<> dist(-100, 100);

    allTransactions.resize(totalOps);

    for (int i = 0; i < totalOps; ++i)
        allTransactions[i] = dist(gen);

    long long expectedBalance = 0;
    for (int value : allTransactions)
        expectedBalance += value;

    std::cout << "\nОжидаемый баланс: " << expectedBalance << "\n\n";

    runTest(threadCount, operations, WITHOUT_SYNCHRONIZATION, expectedBalance);
    runTest(threadCount, operations, ATOMIC, expectedBalance);
    runTest(threadCount, operations, MUTEX, expectedBalance);

    return 0;
}