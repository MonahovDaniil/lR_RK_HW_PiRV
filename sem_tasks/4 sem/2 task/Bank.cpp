#include <vector>
#include <thread>
#include <iostream>

#include "Account.cpp"

template<typename T>
class Bank {

private:

    std::vector<Account<T>> accounts;

    inline void do_transfer(int from, int to, T amount) {

        accounts[from].withdraw(amount);
        accounts[to].deposit(amount);

        std::cout
            << "Поток "
            << std::this_thread::get_id()
            << " переводит "
            << amount
            << " из счета "
            << from
            << " в счет "
            << to
            << std::endl;
    }

public:

    Bank(int n_accounts, T initial_balance) {

        for (int i = 0; i < n_accounts; i++) {
            accounts.emplace_back(initial_balance);
        }
    }

    void transfer(int from, int to, T amount) {

        std::thread([=]() {

            do_transfer(from, to, amount);

        }).detach();
    }

    T total_balance() {

        T sum = 0;

        for (auto& acc : accounts) {
            sum += acc.get_balance();
        }

        return sum;
    }
};