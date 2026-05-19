#include <iostream>
#include <exception>
#include <boost/asio.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/as_tuple.hpp>

namespace asio = boost::asio;
using asio::ip::tcp;

asio::awaitable<void> echo_session(tcp::socket sock) {
    char data[1024];
    try {
        for (;;) {
            auto [ec, n] = co_await sock.async_read_some(asio::buffer(data), asio::as_tuple(asio::use_awaitable));
            
            if (ec == asio::error::eof) {
                std::cout << "Клиент успешно отключился (EOF получено).\n";
                break;
            }
            
            if (ec) {
                throw boost::system::system_error(ec);
            }
            
            co_await asio::async_write(sock, asio::buffer(data, n), asio::use_awaitable);
        }
    } catch (const std::exception& e) {
        std::cerr << "Ошибка в echo_session: " << e.what() << std::endl;
    }
}

asio::awaitable<void> accept_connections(asio::io_context& io_context, short port) {
    tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), port));
    std::cout << "Эхо-сервер запущен и слушает порт " << port << "...\n";
 
    while (true) {
        tcp::socket socket(io_context);
        
        co_await acceptor.async_accept(socket, asio::use_awaitable);
        
        std::cout << "Подключился новый клиент!\n";
        
        asio::co_spawn(io_context, echo_session(std::move(socket)), asio::detached);
    }
}

int main() {
    try {
        asio::io_context io_context;
        
        asio::co_spawn(io_context, accept_connections(io_context, 12345), asio::detached);
        
        io_context.run();
        
    } catch (const std::exception& e) {
        std::cerr << "Критическое исключение в main: " << e.what() << std::endl;
    }
    return 0;
}