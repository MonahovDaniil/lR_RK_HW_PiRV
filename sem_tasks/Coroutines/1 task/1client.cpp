#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <boost/asio/detached.hpp>

namespace asio = boost::asio;
using asio::ip::tcp;

asio::awaitable<void> run_client(std::string host, std::string port) {
    auto executor = co_await asio::this_coro::executor;
    tcp::socket socket(executor);

    tcp::resolver resolver(executor);
    auto endpoints = co_await resolver.async_resolve(host, port, asio::use_awaitable);
    co_await asio::async_connect(socket, endpoints, asio::use_awaitable);
    
    std::cout << "Подключились к серверу!" << std::endl;
    std::cout << "Введите сообщение для получения эхо-ответа или 'q' для выхода" << std::endl;
    std::cout << "\n" << std::endl;

    char reply[1024];
    std::string message;
    
    while (true) {
        std::cout << "> ";
        std::getline(std::cin, message);
        
        if (message == "q") {
            std::cout << "Завершение работы клиента..." << std::endl;
            break;
        }
        
        if (message.empty()) continue;
        
        try {
            co_await asio::async_write(socket, asio::buffer(message), asio::use_awaitable);
            std::cout << "[Отправлено]: " << message << std::endl;
            
            size_t n = co_await socket.async_read_some(asio::buffer(reply), asio::use_awaitable);
            std::cout << "[Получено эхо]: " << std::string(reply, n) << std::endl;
            std::cout << "\n" << std::endl;
            
        } catch (const boost::system::system_error& e) {
            if (e.code() == asio::error::eof) {
                std::cout << "Сервер закрыл соединение." << std::endl;
            } else {
                std::cerr << "Ошибка: " << e.what() << std::endl;
            }
            break;
        }
    }
}

int main() {
    try {
        asio::io_context io_context;
        asio::co_spawn(io_context, run_client("127.0.0.1", "12345"), asio::detached);
        io_context.run();
    } catch (std::exception& e) {
        std::cerr << "Ошибка клиента: " << e.what() << std::endl;
    }
    return 0;
}