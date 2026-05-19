#include "DownloadManager.h"
#include <iostream>
#include <thread>

DownloadManager::DownloadManager(int max_files, int max_chunks)
    : active_downloads(max_files),
      chunk_downloads(max_chunks)
{}

void DownloadManager::add_file(int file_id, int chunks_count)
{
    active_downloads.acquire();

    std::lock_guard<std::mutex> lock(queue_mutex);

    files.emplace_back(file_id);

    auto& file = files.back();

    for(int c = 0; c < chunks_count; c++)
    {
        FileChunk chunk;

        chunk.chunk_id = c;
        chunk.file_id = file_id;
        chunk.size = 100;

        file.chunks.push_back(chunk);
        chunk_queue.push(chunk);
    }

    std::cout << "Файл "
              << file.file_id
              << " добавлен с "
              << file.chunks.size()
              << " чанками\n";
}

inline void DownloadManager::process_chunk(FileChunk chunk)
{
    chunk.download();
}

void DownloadManager::download_worker()
{
    while(true)
    {
        FileChunk chunk;
        bool has_chunk = false;

        {
            std::lock_guard<std::mutex> lock(queue_mutex);

            if(!chunk_queue.empty())
            {
                chunk = chunk_queue.front();
                chunk_queue.pop();
                has_chunk = true;
            }
        }

        if(!has_chunk)
        {
            std::this_thread::yield();
            continue;
        }

        chunk_downloads.acquire();

        std::cout << "Поток "
                  << std::this_thread::get_id()
                  << " скачивает файл "
                  << chunk.file_id
                  << " чанк "
                  << chunk.chunk_id
                  << std::endl;

        process_chunk(chunk);

        std::cout << "Поток "
                  << std::this_thread::get_id()
                  << " завершил файл "
                  << chunk.file_id
                  << " чанк "
                  << chunk.chunk_id
                  << std::endl;

        chunk_downloads.release();

        bool file_complete = false;

        {
            std::lock_guard<std::mutex> lock(queue_mutex);

            auto& file = files[chunk.file_id];

            file.mark_chunk_downloaded();

            if(file.is_complete())
            {
                completed_files++;

                file_complete = true;

                std::cout << "Файл "
                          << file.file_id
                          << " полностью скачан\n";
            }
        }

        if(file_complete)
            active_downloads.release();

        std::this_thread::yield();
    }
}

int DownloadManager::finished_files()
{
    return completed_files.load();
}