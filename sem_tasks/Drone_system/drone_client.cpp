#include <iostream>
#include <string>
#include <memory>
#include <chrono>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

class DroneClient : public std::enable_shared_from_this<DroneClient> {
public:
    DroneClient(boost::asio::io_context& io_context, const std::string& host, short port, int drone_id)
        : socket_(io_context),
          timer_(io_context),
          drone_id_(drone_id),
          x_(10.5 * drone_id),
          y_(20.5 * drone_id) {
        
        tcp::resolver resolver(io_context);
        endpoints_ = resolver.resolve(host, std::to_string(port));
    }

    void connect() {
        auto self(shared_from_this());
        // Шаг 1. Асинхронное подключение к серверу управления
        boost::asio::async_connect(socket_, endpoints_,
            [this, self](boost::system::error_code ec, tcp::endpoint) {
                if (!ec) {
                    std::cout << "[БПЛА " << drone_id_ << "] Успешно подключен к серверу.\n";
                    send_coordinates();
                } else {
                    std::cerr << "[БПЛА " << drone_id_ << "] Ошибка подключения: " << ec.message() << "\n";
                }
            });
    }

private:
    // Шаг 2. Асинхронная отправка текущих координат
    void send_coordinates() {
        auto self(shared_from_this());

        x_ += 0.123;
        y_ += 0.456;

        message_ = "ID:" + std::to_string(drone_id_) + " | Положение: X=" + 
                   std::to_string(x_) + ", Y=" + std::to_string(y_);

        std::cout << "[БПЛА " << drone_id_ << "] Отправка координат...\n";

        boost::asio::async_write(socket_, boost::asio::buffer(message_),
            [this, self](boost::system::error_code ec, std::size_t /*length*/) {
                if (!ec) {
                    read_echo_response();
                } else {
                    std::cerr << "[БПЛА " << drone_id_ << "] Ошибка отправки: " << ec.message() << "\n";
                }
            });
    }

    // Шаг 3. Получение эхо-отклика от сервера
    void read_echo_response() {
        auto self(shared_from_this());

        socket_.async_read_some(boost::asio::buffer(reply_buffer_, max_length),
            [this, self](boost::system::error_code ec, std::size_t length) {
                if (!ec) {
                    std::cout << "[БПЛА " << drone_id_ << "] Получен эхо-отклик: " 
                              << std::string(reply_buffer_, length) << "\n";

                    // Шаг 4. Обновление положения по таймеру
                    timer_.expires_after(std::chrono::seconds(2));
                    timer_.async_wait([this, self](boost::system::error_code ec) {
                        if (!ec) {
                            send_coordinates();
                        }
                    });
                } else {
                    std::cerr << "[БПЛА " << drone_id_ << "] Соединение закрыто сервером или произошла ошибка: " 
                              << ec.message() << "\n";
                }
            });
    }

    tcp::socket socket_;
    tcp::resolver::results_type endpoints_;
    boost::asio::steady_timer timer_;
    int drone_id_;
    double x_, y_;
    std::string message_;
    
    enum { max_length = 1024 };
    char reply_buffer_[max_length];
};

int main() {
    try {
        boost::asio::io_context io_context;

        auto drone1 = std::make_shared<DroneClient>(io_context, "127.0.0.1", 12345, 101);
        auto drone2 = std::make_shared<DroneClient>(io_context, "127.0.0.1", 12345, 102);
        auto drone3 = std::make_shared<DroneClient>(io_context, "127.0.0.1", 12345, 103);

        drone1->connect();
        drone2->connect();
        drone3->connect();

        std::cout << "[Система] Запущено 3 беспилотника. Нажмите Ctrl+C для выхода.\n";
        
        io_context.run();

    } catch (std::exception& e) {
        std::cerr << "Исключение в клиенте: " << e.what() << "\n";
    }

    return 0;
}