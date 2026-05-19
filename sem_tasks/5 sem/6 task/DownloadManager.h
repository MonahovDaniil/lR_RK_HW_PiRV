#pragma once

#include <queue>
#include <vector>
#include <mutex>
#include <atomic>
#include <semaphore>
#include "FileChunk.h"
#include "FileDownload.h"

class DownloadManager
{
private:

    std::queue<FileChunk> chunk_queue;

    std::counting_semaphore<100> active_downloads;
    std::counting_semaphore<100> chunk_downloads;

    std::mutex queue_mutex;

    std::atomic<int> completed_files{0};

    std::vector<FileDownload> files;

public:

    DownloadManager(int max_files, int max_chunks);

    void add_file(int file_id, int chunks_count);

    void download_worker();

    inline void process_chunk(FileChunk chunk);

    int finished_files();
};