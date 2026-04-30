#include "server4.hpp"
#include <iostream>
#include <chrono>
#include <algorithm>
#include <cctype>

using namespace std::chrono_literals;

Session::Session(tcp::socket socket,
                 boost::asio::strand<boost::asio::io_context::executor_type> strand,
                 std::vector<std::string>& log,
                 boost::asio::thread_pool& pool)
    : socket_(std::move(socket)),
      strand_(std::move(strand)),
      log_(log),
      pool_(pool),
      timer_(socket_.get_executor()) {}

void Session::start() {
    arm_timer();
    do_read();
}

void Session::arm_timer() {
    timer_.expires_after(10s);
    auto self = shared_from_this();
    timer_.async_wait(boost::asio::bind_executor(strand_,
        [this, self](boost::system::error_code ec) {
            if (!ec) {
                std::cout << "Таймаут бездействия, закрываем соединение.\n";
                boost::system::error_code ignored;
                socket_.shutdown(tcp::socket::shutdown_both, ignored);
                socket_.close(ignored);
                is_closed_ = true;
            }
        }));
}

void Session::cancel_timer() {
    timer_.cancel();
}

void Session::do_read() {
    auto self = shared_from_this();
    socket_.async_read_some(boost::asio::buffer(data_, max_length),
        boost::asio::bind_executor(strand_,
            [this, self](boost::system::error_code ec, std::size_t length) {
                if (ec) {
                    if (is_closed_) return;
                    
                    if (ec == boost::asio::error::eof)
                        std::cerr << "Клиент отключен.\n";
                    else if (ec == boost::asio::error::operation_aborted)
                        ;
                    else
                        std::cerr << "Ошибку чтения. Соединение потеряно.\n";
                    cancel_timer();
                    return;
                }
                cancel_timer();
                std::string request(data_, length);
                while (!request.empty() &&
                       (request.back() == '\n' || request.back() == '\r'))
                    request.pop_back();
                process_request(request);
            }));
}

void Session::process_request(const std::string& request) {
    if (request.empty() || !std::all_of(request.begin(), request.end(), ::isdigit)) {
        std::string error = "Ошибка: введите целое неотрицательное число.\n";
        boost::asio::post(strand_, [this, self = shared_from_this(), error]() {
            do_write(error);
        });
        return;
    }

    unsigned long n = std::stoul(request);

    if (n > 20) {
        std::string error = "Ошибка: поддерживаются числа от 0 до 20.\n";
        boost::asio::post(strand_, [this, self = shared_from_this(), error]() {
            do_write(error);
        });
        return;
    }

    auto self = shared_from_this();
    boost::asio::post(pool_, [this, self, n]() {
        unsigned long long result = 1;
        for (unsigned long i = 2; i <= n; ++i)
            result *= i;

        std::string response = "Факториал " + std::to_string(n) +
                               " = " + std::to_string(result) + "\n";

        boost::asio::post(strand_, [this, self, response]() {
            log_.push_back(response);
            std::cout << "[ЛОГ] " << response;
            do_write(response);
        });
    });
}

void Session::do_write(const std::string& msg) {
    if (is_closed_) return;
    auto self = shared_from_this();
    boost::asio::async_write(socket_, boost::asio::buffer(msg),
        boost::asio::bind_executor(strand_,
            [this, self](boost::system::error_code ec, std::size_t) {
                if (ec) {
                    std::cerr << "Ошибка отправки" << "\n";
                    return;
                }
                if (!is_closed_) {
                    arm_timer();
                    do_read();
                }
            }));
}

Server::Server(boost::asio::io_context& io, unsigned short port, int threads)
    : acceptor_(io, tcp::endpoint(tcp::v4(), port)),
      strand_(io.get_executor()),
      pool_(threads),
      io_(io) {
    do_accept();
}

void Server::do_accept() {
    acceptor_.async_accept(
        [this](boost::system::error_code ec, tcp::socket socket) {
            if (!ec) {
                std::make_shared<Session>(std::move(socket),
                                          strand_, log_, pool_)->start();
            }
            if (acceptor_.is_open())
                do_accept();
        });
}

void Server::stop() {
    acceptor_.close();
    pool_.join();
    io_.stop();
}