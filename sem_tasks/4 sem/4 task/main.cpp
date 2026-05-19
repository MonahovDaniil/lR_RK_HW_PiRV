#include <iostream>
#include <thread>
#include <chrono>

#include "Logger.cpp"

int main() {

    Logger<std::string> logger("log.txt");

    int threads = 5;
    int messages_per_thread = 10;

    for (int i = 0; i < threads; i++) {

        std::thread([i, &logger, messages_per_thread]() {

            for (int j = 0; j < messages_per_thread; j++) {

                logger.log(
                    "Сообщение " + std::to_string(j) +
                    " от работника " + std::to_string(i)
                );

                std::this_thread::sleep_for(
                    std::chrono::milliseconds(100)
                );
            }

        }).detach();
    }

    std::this_thread::sleep_for(std::chrono::seconds(3));

    return 0;
}