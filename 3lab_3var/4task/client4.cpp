#include "client4.hpp"
#include <boost/asio.hpp>
#include <iostream>
#include <string>
#include <algorithm>
#include <cctype>

using boost::asio::ip::tcp;

namespace {
    bool is_valid_number(const std::string& s) {
        if (s.empty()) return false;
        if (!std::all_of(s.begin(), s.end(), ::isdigit))
            return false;
        if (s.size() > 1 && s[0] == '0')
            return false;
        return true;
    }
}

void run_client() {
    try {
        boost::asio::io_context io_context;
        tcp::socket socket(io_context);
        tcp::resolver resolver(io_context);
        auto endpoints = resolver.resolve("127.0.0.1", "12345");
        boost::asio::connect(socket, endpoints);

        std::cout << "Подключено к серверу.\n"
                  << "Введите число для факториала (или 'q' для выхода):\n";

        while (true) {
            std::string input;
            std::getline(std::cin, input);

            if (input == "q" || input == "Q") break;

            if (!is_valid_number(input)) {
                std::cout << "Ошибка: введите целое неотрицательное число "
                             "(без пробелов и ведущих нулей).\n";
                continue;
            }

            std::string request = input + "\n";
            boost::asio::write(socket, boost::asio::buffer(request));

            boost::asio::streambuf response;
            boost::asio::read_until(socket, response, '\n');
            std::istream is(&response);
            std::string line;
            std::getline(is, line);
            std::cout << "Ответ: " << line << std::endl;
        }
        socket.close();
        std::cout << "Соединение закрыто.\n";
    } catch (const std::exception& e) {
        std::cerr << "Ошибка клиента" << std::endl;
    }
}

int main() {
    run_client();
    return 0;
}