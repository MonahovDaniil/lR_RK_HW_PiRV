#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <thread>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

// Шаг 1. Глобальная переменная-счётчик сообщений (общий ресурс между потоками)
int global_message_count = 0;

// Шаг 2. Strand для синхронизации доступа к счётчику из нескольких потоков без mutex'ов
std::shared_ptr<boost::asio::strand<boost::asio::io_context::executor_type>> strand;

// Шаг 3. Класс Session для общения с одним беспилотником
class Session : public std::enable_shared_from_this<Session> {
public:
    Session(tcp::socket socket) : socket_(std::move(socket)) {}

    void start() {
        do_read();
    }

private:
    // Шаг 4. Чтение данных от беспилотника
    void do_read() {
        auto self(shared_from_this());
        
        socket_.async_read_some(boost::asio::buffer(data_, max_length),
            boost::asio::bind_executor(*strand,
                [this, self](boost::system::error_code ec, std::size_t length) {
                    if (!ec) {
                        std::cout << "[Сервер] Получены координаты: " << std::string(data_, length) << "\n";
                        
                        ++global_message_count;
                        std::cout << "[Сервер] Общее количество обработанных сообщений: " 
                                  << global_message_count << "\n\n";
                        
                        do_write(length);
                    }
                }));
    }

    // Шаг 5. Реализация отправки эхо-ответа обратно клиенту
    void do_write(std::size_t length) {
        auto self(shared_from_this());
        
        boost::asio::async_write(socket_, boost::asio::buffer(data_, length),
            boost::asio::bind_executor(*strand,
                [this, self](boost::system::error_code ec, std::size_t /*length*/) {
                    if (!ec) {
                        do_read();
                    }
                }));
    }

    tcp::socket socket_;
    enum { max_length = 1024 };
    char data_[max_length];
};

// Шаг 6. Класс Server для прослушивания порта и принятия новых подключений
class Server {
public:
    Server(boost::asio::io_context& io_context, short port)
        : acceptor_(io_context, tcp::endpoint(tcp::v4(), port)) {
        do_accept();
    }

private:
    void do_accept() {
        acceptor_.async_accept(
            boost::asio::make_strand(acceptor_.get_executor()),
            [this](boost::system::error_code ec, tcp::socket socket) {
                if (!ec) {
                    std::make_shared<Session>(std::move(socket))->start();
                }
                do_accept();
            });
    }

    tcp::acceptor acceptor_;
};

// Шаг 7. Главный поток программы (main)
int main() {
    try {
        boost::asio::io_context io_context;

        strand = std::make_shared<boost::asio::strand<boost::asio::io_context::executor_type>>(
            io_context.get_executor()
        );

        short port = 12345;
        Server server(io_context, port);
        std::cout << "[Сервер] Запущен на порту " << port << ". Ожидание беспилотников...\n";

        std::vector<std::thread> threads;
        const int num_threads = 4;
        
        for (int i = 0; i < num_threads; ++i) {
            threads.emplace_back([&io_context]() {
                io_context.run();
            });
        }

        for (auto& t : threads) {
            t.join();
        }

    } catch (std::exception& e) {
        std::cerr << "Исключение (Ошибка): " << e.what() << "\n";
    }

    return 0;
}