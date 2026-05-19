#include <iostream>
#include <vector>
#include <thread>
#include <atomic>
#include <random>
#include <stdexcept>
#include <boost/asio.hpp>

namespace asio = boost::asio;

class BankAccount {
public:
    explicit BankAccount(asio::any_io_executor executor)
        : strand_(asio::make_strand(executor)), balance_(0) {}

    asio::awaitable<void> async_deposit(int64_t amount) {
        co_await asio::post(strand_, asio::use_awaitable);
        balance_ += amount;
    }

    asio::awaitable<void> async_withdraw(int64_t amount) {
        co_await asio::post(strand_, asio::use_awaitable);
        if (balance_ < amount) {
            throw std::invalid_argument("Insufficient funds");
        }
        balance_ -= amount;
    }

    asio::awaitable<int64_t> async_get_balance() {
        co_await asio::post(strand_, asio::use_awaitable);
        co_return balance_;
    }

private:
    asio::strand<asio::any_io_executor> strand_;
    int64_t balance_;
};

asio::awaitable<void> transaction_worker(BankAccount& account, 
                                         std::atomic<int64_t>& total_deposited, 
                                         std::atomic<int64_t>& total_withdrawn,
                                         std::atomic<int>& active_coroutines) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int64_t> dist(1, 100);

    try {
        for (int i = 0; i < 10; ++i) {
            int64_t dep = dist(gen);
            co_await account.async_deposit(dep);
            total_deposited += dep;

            int64_t with = dist(gen);
            try {
                co_await account.async_withdraw(with);
                total_withdrawn += with;
            } catch (const std::invalid_argument&) {
                // Игнорируем ошибку нехватки средств при симуляции
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Ошибка в корутине: " << e.what() << std::endl;
    }

    --active_coroutines;
}

int main() {
    try {
        asio::io_context io_context;
        BankAccount account(io_context.get_executor());

        std::atomic<int64_t> total_deposited{0};
        std::atomic<int64_t> total_withdrawn{0};
        std::atomic<int> active_coroutines{100};

        for (int i = 0; i < 100; ++i) {
            asio::co_spawn(io_context, 
                           transaction_worker(account, total_deposited, total_withdrawn, active_coroutines), 
                           asio::detached);
        }

        std::vector<std::thread> threads;
        for (int i = 0; i < 4; ++i) {
            threads.emplace_back([&io_context]() {
                io_context.run();
            });
        }

        for (auto& t : threads) {
            t.join();
        }

        io_context.restart();
        int64_t final_balance = 0;
        asio::co_spawn(io_context, [&]() -> asio::awaitable<void> {
            final_balance = co_await account.async_get_balance();
            co_return;
        }, asio::detached);
        io_context.run();

        int64_t expected_balance = total_deposited - total_withdrawn;

        std::cout << "Результаты проверки:" << std::endl;
        std::cout << "Всего внесено:   " << total_deposited << std::endl;
        std::cout << "Всего снято:     " << total_withdrawn << std::endl;
        std::cout << "Ожидаемый баланс: " << expected_balance << std::endl;
        std::cout << "Финальный баланс: " << final_balance << std::endl;

        if (final_balance == expected_balance) {
            std::cout << "УСПЕХ: Балансы совпадают! Состояние гонки отсутствует." << std::endl;
        } else {
            std::cout << "ОШИБКА: Обнаружено состояние гонки!" << std::endl;
        }

    } catch (const std::exception& e) {
        std::cerr << "Критическое исключение: " << e.what() << std::endl;
    }
    return 0;
}