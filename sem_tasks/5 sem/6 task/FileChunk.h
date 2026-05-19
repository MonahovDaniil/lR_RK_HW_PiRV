#pragma once
#include <chrono>
#include <thread>

struct FileChunk
{
    int chunk_id;
    int file_id;
    size_t size;

    void download()
    {
        std::this_thread::sleep_for(
            std::chrono::milliseconds(100 + size)
        );
    }
};