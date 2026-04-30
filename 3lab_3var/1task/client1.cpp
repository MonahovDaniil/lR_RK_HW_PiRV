#include <boost/asio.hpp>
#include <iostream>
#include <string>

using boost::asio::ip::tcp;

int main() {
    try {
        boost::asio::io_context io_context;
        
        tcp::socket socket(io_context);
        tcp::endpoint endpoint(boost::asio::ip::make_address("127.0.0.1"), 12345);
        socket.connect(endpoint);

        std::cout << "Введите сообщение для отправки: ";
        std::string message;
        std::getline(std::cin, message);
        message += "\n";

        boost::asio::write(socket, boost::asio::buffer(message));

        boost::asio::streambuf response_buffer;
        boost::asio::read_until(socket, response_buffer, '\n');

        std::istream response_stream(&response_buffer);
        std::string response;
        std::getline(response_stream, response);

        std::cout << "Ответ сервера: " << response << std::endl;

    } catch (std::exception& e) {
        std::cerr << "Ошибка клиента: " << e.what() << std::endl;
    }
    return 0;
}