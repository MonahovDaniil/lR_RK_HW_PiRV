#include <boost/asio.hpp>
#include <iostream>
#include <string>
#include <algorithm>
#include <cctype>

using boost::asio::ip::tcp;

int main() {
    try {
        boost::asio::io_context io_context;
        
        tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 12345)); 
        
        std::cout << "Сервер запущен на порту 12345..." << std::endl;

        for (;;) {
            tcp::socket socket(io_context);
            acceptor.accept(socket);

            boost::asio::streambuf buffer;
            boost::asio::read_until(socket, buffer, '\n');

            std::istream input_stream(&buffer);
            std::string message;
            std::getline(input_stream, message);

            size_t length = message.length();
            std::string upper_message = message;
            std::transform(upper_message.begin(), upper_message.end(), upper_message.begin(), 
                           [](unsigned char c) { return std::toupper(c); });

            std::string response = std::to_string(length) + ": " + upper_message + "\n";
            
            boost::asio::write(socket, boost::asio::buffer(response)); 
        }
    } catch (std::exception& e) {
        std::cerr << "Ошибка сервера: " << e.what() << std::endl;
    }
    return 0;
}