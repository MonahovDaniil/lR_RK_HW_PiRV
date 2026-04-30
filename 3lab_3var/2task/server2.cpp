#include <boost/asio.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <memory>

using boost::asio::ip::tcp;

class Session : public std::enable_shared_from_this<Session> {
public:
    Session(tcp::socket socket) : socket_(std::move(socket)) {}

    void start() {
        do_read();
    }

private:
    void do_read() {
        auto self(shared_from_this());
        socket_.async_read_some(boost::asio::buffer(data_),
            [this, self](boost::system::error_code ec, std::size_t length) {
                if (!ec) {
                    std::string request(data_, length);
                    process_request(request);
                }
            });
    }

    void process_request(const std::string& request) {
        auto self(shared_from_this());
        boost::asio::post(socket_.get_executor(), [this, self, request]() {
            std::istringstream iss(request);
            std::vector<int> numbers;
            int n;
            while (iss >> n) {
                numbers.push_back(n);
            }

            std::string response;
            if (numbers.empty()) {
                response = "Ошибка: список чисел пуст\n";
            } else {
                auto max_it = std::max_element(numbers.begin(), numbers.end());
                response = "Максимум: " + std::to_string(*max_it) + "\n";
            }
            do_write(response);
        });
    }

    void do_write(const std::string& response) {
        auto self(shared_from_this());
        boost::asio::async_write(socket_, boost::asio::buffer(response),
            [this, self](boost::system::error_code ec, std::size_t) {
                if (!ec) {
                    do_read();
                }
            });
    }

    tcp::socket socket_;
    char data_[1024];
};

class Server {
public:
    Server(boost::asio::io_context& io_context, short port)
        : acceptor_(io_context, tcp::endpoint(tcp::v4(), port)) {
        do_accept();
    }

private:
    void do_accept() {
        acceptor_.async_accept(
            [this](boost::system::error_code ec, tcp::socket socket) {
                if (!ec) {
                    std::make_shared<Session>(std::move(socket))->start();
                }
                do_accept();
            });
    }
    tcp::acceptor acceptor_;
};

int main() {
    try {
        boost::asio::io_context io_context;
        Server s(io_context, 12345);
        std::cout << "Асинхронный сервер запущен на порту 12345...\n";
        io_context.run();
    } catch (std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
    }
    return 0;
}