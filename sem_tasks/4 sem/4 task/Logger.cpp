#include <iostream>
#include <fstream>
#include <mutex>
#include <thread>
#include <sstream>

template<typename T>
class Logger {

private:
    std::ofstream log_file;
    std::mutex mtx;

    inline std::string to_string_any(const T& value) {
        std::stringstream ss;
        ss << value;
        return ss.str();
    }

public:

    Logger(const std::string& filename) {
        log_file.open(filename);
    }

    ~Logger() {
        if (log_file.is_open())
            log_file.close();
    }

    void log(const T& message) {

        std::lock_guard<std::mutex> lock(mtx);

        std::string msg = to_string_any(message);

        std::cout
            << "Поток "
            << std::this_thread::get_id()
            << ": "
            << msg
            << std::endl;

        log_file
            << "Поток "
            << std::this_thread::get_id()
            << ": "
            << msg
            << std::endl;
    }
};