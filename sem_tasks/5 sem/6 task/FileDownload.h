#pragma once
#include <vector>
#include <atomic>
#include "FileChunk.h"

class FileDownload
{
public:

    int file_id;
    std::vector<FileChunk> chunks;
    std::atomic<int> downloaded_chunks{0};

    FileDownload(int id)
    {
        file_id = id;
    }

    bool is_complete()
    {
        return downloaded_chunks == chunks.size();
    }

    void mark_chunk_downloaded()
    {
        downloaded_chunks++;
    }

    FileDownload(const FileDownload&) = delete;
    FileDownload& operator=(const FileDownload&) = delete;
};