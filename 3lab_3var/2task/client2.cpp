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

        std::cout << "Введите числа через пробел: ";
        std::string msg;
        std::getline(std::cin, msg);
        
        boost::asio::write(socket, boost::asio::buffer(msg));

        char reply[1024];
        size_t len = socket.read_some(boost::asio::buffer(reply));
        std::cout << "Ответ от сервера: " << std::string(reply, len);

    } catch (std::exception& e) {
        std::cerr << "Ошибка клиента: " << e.what() << std::endl;
    }
    return 0;
}