#include <iostream>
#include <vector>
#include <random>
#include <future>
#include <fstream>
#include <atomic>
#include <chrono>

const int N = 1000;
const int BLOCK_SIZE = 250;

std::vector<std::vector<int>> A, B, C;

std::atomic<int> blocksProcessed(0);

long long processBlock(int rowStart, int colStart) {
    long long localSum = 0;

    for (int i = rowStart; i < rowStart + BLOCK_SIZE; i++) {
        for (int j = colStart; j < colStart + BLOCK_SIZE; j++) {
            C[i][j] = A[i][j] + B[i][j];
            localSum += C[i][j];
        }
    }

    blocksProcessed++;
    return localSum;
}

int main() {
    A.resize(N, std::vector<int>(N));
    B.resize(N, std::vector<int>(N));
    C.resize(N, std::vector<int>(N));

    std::mt19937 gen(42);
    std::uniform_int_distribution<> dist(1, 10);

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            A[i][j] = dist(gen);
            B[i][j] = dist(gen);
        }
    }

    auto start = std::chrono::high_resolution_clock::now();

    std::vector<std::future<long long>> futures;

    for (int i = 0; i < N; i += BLOCK_SIZE) {
        for (int j = 0; j < N; j += BLOCK_SIZE) {
            futures.push_back(
                std::async(std::launch::async, processBlock, i, j)
            );
        }
    }

    long long totalSum = 0;
    for (auto& f : futures) {
        totalSum += f.get();
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    std::ofstream out("C:\\Users\\PC\\vsprojecct\\labs\\HW1_2var\\result_matrix.txt");
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            out << C[i][j] << " ";
        }
        out << "\n";
    }
    out.close();

    std::cout << "Сумма элементов матрицы C: " << totalSum << "\n";
    std::cout << "Обработано блоков: " << blocksProcessed << "\n";
    std::cout << "Время выполнения: " << duration.count() << " мс\n";



    return 0;
}