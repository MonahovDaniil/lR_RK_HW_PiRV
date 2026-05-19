#include <iostream>
#include <thread>
#include <chrono>

#include "Buffer.cpp"

int main() {

    Buffer<int> buffer(5);

    int produce_count = 20;
    int consume_count = 20;

    int producers = 3;
    int consumers = 3;

    for (int p = 0; p < producers; p++) {

        std::thread([&buffer, produce_count, p]() {

            for (int i = 0; i < produce_count; i++) {

                buffer.produce(i + p * 100);

                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }

        }).detach();
    }

    for (int c = 0; c < consumers; c++) {

        std::thread([&buffer, consume_count]() {

            for (int i = 0; i < consume_count; i++) {

                buffer.consume();

                std::this_thread::sleep_for(std::chrono::milliseconds(150));
            }

        }).detach();
    }

    std::this_thread::sleep_for(std::chrono::seconds(10));

    return 0;
}