#include <iostream>
#include <thread>
#include <random>
#include "DownloadManager.h"

int main()
{
    const int MAX_ACTIVE_FILES = 2;
    const int MAX_CHUNK_DOWNLOADS = 4;
    const int WORKERS = 4;

    DownloadManager manager(MAX_ACTIVE_FILES, MAX_CHUNK_DOWNLOADS);

    for(int i=0;i<WORKERS;i++)
    {
        std::thread([&manager]{
            manager.download_worker();
        }).detach();
    }

    std::mt19937 gen(42);

    std::uniform_int_distribution<int> chunks_dist(3,6);
    std::uniform_int_distribution<int> size_dist(50,200);

    for(int f = 0; f < 5; f++)
    {
        manager.add_file(f, 5);

        std::this_thread::sleep_for(
            std::chrono::milliseconds(200)
        );
    }

    std::this_thread::sleep_for(
        std::chrono::seconds(8)
    );

    std::cout << "\nЗавершенные файлы: "
              << manager.finished_files()
              << std::endl;
}