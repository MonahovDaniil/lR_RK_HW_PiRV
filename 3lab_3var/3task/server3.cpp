#include <boost/asio.hpp>
#include <iostream>
#include <string>
#include <memory>
#include <sstream>

using boost::asio::ip::tcp;

class Session : public std::enable_shared_from_this<Session> {
public:
    Session(tcp::socket socket) 
        : socket_(std::move(socket)), timer_(socket_.get_executor()) {}

    void start() {
        do_read();
    }

private:
    void do_read() {
        auto self(shared_from_this());
        boost::asio::async_read_until(socket_, buffer_, '\n',
            [this, self](boost::system::error_code ec, std::size_t length) {
                if (!ec) {
                    std::istream is(&buffer_);
                    std::string line;
                    std::getline(is, line);
                    process_command(line);
                }
            });
    }

    void process_command(const std::string& line) {
        std::istringstream iss(line);
        std::string cmd;
        double seconds = -1;

        if (!(iss >> cmd >> seconds) || cmd != "timer") {
            send_error("Ошибка: некорректный ввод. Используйте: timer <число>\n");
            return;
        }

        if (seconds <= 0) {
            send_error("Ошибка: количество секунд должно быть положительным числом\n");
            return;
        }

        int int_seconds = static_cast<int>(seconds);
        std::string response = "Ready in " + std::to_string(int_seconds) + " sec\n";
        auto self(shared_from_this());
        
        boost::asio::async_write(socket_, boost::asio::buffer(response),
            [this, self, int_seconds](boost::system::error_code ec, std::size_t) {
                if (!ec) {
                    timer_.expires_after(boost::asio::chrono::seconds(int_seconds));
                    timer_.async_wait([this, self](boost::system::error_code ec) {
                        if (!ec) {
                            send_done();
                        }
                    });
                }
            });
    }

    void send_error(const std::string& error_msg) {
        auto self(shared_from_this());
        boost::asio::async_write(socket_, boost::asio::buffer(error_msg),
            [this, self](boost::system::error_code ec, std::size_t) {
                if (!ec) {
                    do_read();
                }
            });
    }

    void send_done() {
        auto self(shared_from_this());
        std::string msg = "Done!\n";
        boost::asio::async_write(socket_, boost::asio::buffer(msg),
            [this, self](boost::system::error_code ec, std::size_t) {
                if (!ec) {
                    do_read();
                }
            });
    }

    tcp::socket socket_;
    boost::asio::steady_timer timer_;
    boost::asio::streambuf buffer_;
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
        std::cout << "Сервер запущен на порту 12345..." << std::endl;
        io_context.run();
    } catch (std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
    }
    return 0;
}