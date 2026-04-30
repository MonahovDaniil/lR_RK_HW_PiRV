#include <boost/asio.hpp>
#include <iostream>
#include <string>

using boost::asio::ip::tcp;

int main() {
    try {
        boost::asio::io_context io_context;
        tcp::socket socket(io_context);
        tcp::resolver resolver(io_context);
        boost::asio::connect(socket, resolver.resolve("127.0.0.1", "12345"));

        std::cout << "Введите команду (например, timer 5): ";
        std::string msg;
        std::getline(std::cin, msg);
        msg += "\n";

        boost::asio::write(socket, boost::asio::buffer(msg));

        for (int i = 0; i < 2; ++i) {
            boost::asio::streambuf buffer;
            boost::asio::read_until(socket, buffer, '\n');
            std::istream is(&buffer);
            std::string response;
            std::getline(is, response);
            std::cout << "Сервер: " << response << std::endl;
        }

    } catch (std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
    }
    return 0;
}