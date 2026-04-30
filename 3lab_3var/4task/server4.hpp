#pragma once

#include <boost/asio.hpp>
#include <memory>
#include <string>
#include <vector>

using boost::asio::ip::tcp;

class Session : public std::enable_shared_from_this<Session> {
public:
    Session(tcp::socket socket,
            boost::asio::strand<boost::asio::io_context::executor_type> strand,
            std::vector<std::string>& log,
            boost::asio::thread_pool& pool);
    void start();

private:
    void do_read();
    void do_write(const std::string& msg);
    void process_request(const std::string& request);
    void arm_timer();
    void cancel_timer();

    tcp::socket socket_;
    boost::asio::strand<boost::asio::io_context::executor_type> strand_;
    std::vector<std::string>& log_;
    boost::asio::thread_pool& pool_;
    boost::asio::steady_timer timer_;
    static constexpr std::size_t max_length = 1024;
    char data_[max_length];
    bool is_closed_ = false;
};

class Server {
public:
    Server(boost::asio::io_context& io, unsigned short port, int threads);
    void stop();

private:
    void do_accept();

    tcp::acceptor acceptor_;
    boost::asio::strand<boost::asio::io_context::executor_type> strand_;
    std::vector<std::string> log_;
    boost::asio::thread_pool pool_;
    boost::asio::io_context& io_;
};