#include <boost/asio.hpp>
#include <iostream>
#include <thread>
#include <vector>
#include "server4.hpp"

int main() {
    const unsigned int hw_threads = std::thread::hardware_concurrency();
    const unsigned short port = 12345;

    try {
        boost::asio::io_context io_context;

        Server server(io_context, port, hw_threads);

        std::vector<std::thread> io_threads;
        for (unsigned int i = 0; i < hw_threads; ++i) {
            io_threads.emplace_back([&io_context]() {
                io_context.run();
            });
        }

        std::cout << "Сервер запущен на порту " << port
                  << " (потоков: " << hw_threads << ")\n"
                  << "Для остановки нажмите ENTER...\n";

        std::cin.get();

        std::cout << "Останавливаем сервер...\n";
        server.stop();

        for (auto& t : io_threads)
            t.join();

        std::cout << "Сервер завершил работу.\n";
    } catch (const std::exception& e) {
        std::cerr << "Ошибка" << std::endl;
        return 1;
    }
    return 0;
}