#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <iostream>

template<typename T>
class MatrixProcessor {
private:
    std::vector<std::vector<T>> matrix;
    size_t numThreads;
    mutable std::mutex mtx;
    std::condition_variable cv;
    size_t finishedThreads = 0;

public:
    MatrixProcessor(const std::vector<std::vector<T>>& mat, size_t threads)
        : matrix(mat), numThreads(threads) {}

    void apply(std::function<T(T)> func) {

    size_t totalRows = matrix.size();
    if (totalRows == 0) return;

    size_t threads = std::min(numThreads, totalRows);
    size_t rowsPerThread = (totalRows + threads - 1) / threads;

    for (size_t t = 0; t < threads; ++t) {

        size_t startRow = t * rowsPerThread;
        size_t endRow = std::min(startRow + rowsPerThread, totalRows);

        std::thread([this, startRow, endRow, &func]() {

            {
                std::lock_guard<std::mutex> lock(mtx);
                std::cout << "[Поток " << std::this_thread::get_id()
                          << "] Начал обработку строк с "
                          << startRow << " по " << endRow - 1 << std::endl;
            }

            for (size_t i = startRow; i < endRow; ++i) {
                for (size_t j = 0; j < matrix[i].size(); ++j) {
                    matrix[i][j] = func(matrix[i][j]);
                    std::this_thread::yield();
                }
            }

            {
                std::lock_guard<std::mutex> lock(mtx);
                finishedThreads++;
                std::cout << "[Поток " << std::this_thread::get_id()
                          << "] Завершил обработку строк с "
                          << startRow << " по " << endRow - 1 << std::endl;
            }

            cv.notify_one();

        }).detach();
    }

    std::unique_lock<std::mutex> lock(mtx);
    cv.wait(lock, [&]() { return finishedThreads == threads; });
}

    void printMatrix() const {
        std::lock_guard<std::mutex> lock(mtx);
        std::cout << "Матрица: " << std::endl;
        for (const auto& row : matrix) {
            for (const auto& val : row) {
                std::cout << val << " ";
            }
            std::cout << std::endl;
        }
    }
};