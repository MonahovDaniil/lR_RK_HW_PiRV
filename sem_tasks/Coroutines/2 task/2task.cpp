#include <iostream>
#include <string>
#include <variant>
#include <boost/asio.hpp>
#include <boost/asio/experimental/awaitable_operators.hpp>

namespace asio = boost::asio;
using asio::ip::tcp;
using namespace boost::asio::experimental::awaitable_operators;

asio::awaitable<std::string> read_from(tcp::socket& sock, std::string name) {
    if (!sock.is_open()) {
        asio::steady_timer timer(co_await asio::this_coro::executor, asio::steady_timer::time_point::max());
        try {
            co_await timer.async_wait(asio::use_awaitable);
        } catch (const boost::system::system_error& e) {
            if (e.code() == asio::error::operation_aborted) {
                throw;
            }
        }
        co_return "";
    }

    char data[1024];
    try {
        size_t n = co_await sock.async_read_some(asio::buffer(data), asio::use_awaitable);
        co_return "[" + name + "]: " + std::string(data, n);
    } catch (const boost::system::system_error& e) {
        if (e.code() == asio::error::operation_aborted) {
            throw;
        }
        sock.close();
        if (e.code() == asio::error::eof) {
            co_return "[" + name + "] отключился\n";
        }
        co_return "[" + name + "] ошибка: " + e.what() + "\n";
    }
}

asio::awaitable<void> multiplexer(tcp::socket sock1, tcp::socket sock2) {
    for (;;) {
        if (!sock1.is_open() && !sock2.is_open()) {
            std::cout << "Оба сокета закрыты. Завершение работы мультиплексора.\n";
            break;
        }

        auto result = co_await (read_from(sock1, "sock1") || read_from(sock2, "sock2"));
        
        std::visit([](const std::string& val) {
            if (!val.empty()) {
                std::cout << val << std::flush;
            }
        }, result);
    }
}

asio::awaitable<void> start_scenario(asio::io_context& io_context) {
    tcp::acceptor acceptor1(io_context, tcp::endpoint(tcp::v4(), 12345));
    tcp::acceptor acceptor2(io_context, tcp::endpoint(tcp::v4(), 12346));

    std::cout << "Ожидание подключения sock1 на порту 12345...\n";
    tcp::socket sock1(io_context);
    co_await acceptor1.async_accept(sock1, asio::use_awaitable);

    std::cout << "Ожидание подключения sock2 на порту 12346...\n";
    tcp::socket sock2(io_context);
    co_await acceptor2.async_accept(sock2, asio::use_awaitable);

    co_await multiplexer(std::move(sock1), std::move(sock2));
}

int main() {
    try {
        asio::io_context io_context;
        asio::co_spawn(io_context, start_scenario(io_context), asio::detached);
        io_context.run();
    } catch (const std::exception& e) {
        std::cerr << "Критическое исключение в main: " << e.what() << std::endl;
    }
    return 0;
}