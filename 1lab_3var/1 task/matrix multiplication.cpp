#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include <boost/thread.hpp>

void fillMatrix(std::vector<std::vector<double>>& m, int N) {
    std::mt19937 gen(42);
    std::uniform_real_distribution<> dis(0.0, 10.0);

    for (int i = 0; i < N; ++i)
        for (int j = 0; j < N; ++j)
            m[i][j] = dis(gen);
}

void multiplyRow(const std::vector<std::vector<double>>& A, const std::vector<std::vector<double>>& B,
                 std::vector<std::vector<double>>& C, int N, int row) {
    for (int j = 0; j < N; ++j) {
        C[row][j] = 0;
        for (int k = 0; k < N; ++k) {
            C[row][j] += A[row][k] * B[k][j];
        }
    }
}

void multiplySingle(const std::vector<std::vector<double>>& A, const std::vector<std::vector<double>>& B,
     std::vector<std::vector<double>>& C, int N) {
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            C[i][j] = 0;
            for (int k = 0; k < N; ++k) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

void multiplyMulti(const std::vector<std::vector<double>>& A, const std::vector<std::vector<double>>& B,
                   std::vector<std::vector<double>>& C, int N) {
    std::vector<boost::thread> threads;

    for (int i = 0; i < N; ++i) {
        threads.emplace_back(multiplyRow, std::cref(A), std::cref(B),
                             std::ref(C), N, i);
    }

    for (auto& t : threads)
        t.join();
}

int main() {
    int N = 500;

    std::vector<std::vector<double>> A(N, std::vector<double>(N));
    std::vector<std::vector<double>> B(N, std::vector<double>(N));
    std::vector<std::vector<double>> C(N, std::vector<double>(N));

    fillMatrix(A, N);
    fillMatrix(B, N);

    auto start_multi = std::chrono::high_resolution_clock::now();

    multiplyMulti(A, B, C, N);

    auto end_multi = std::chrono::high_resolution_clock::now();

    auto diff_multi = std::chrono::duration<double>(end_multi - start_multi).count();

    std::cout << "Время многопоточного выполнения: "
              << diff_multi
              << " секунд\n";


    auto start_single = std::chrono::high_resolution_clock::now();

    multiplySingle(A, B, C, N);

    auto end_single = std::chrono::high_resolution_clock::now();

    auto diff_single = std::chrono::duration<double>(end_single - start_single).count();

    std::cout << "Время однопоточного выполнения: "
              << diff_single
              << " секунд\n";

    std::cout << "Многопоточное выполнение в " << diff_single / diff_multi << " раз быстрее однопоточного\n";

    return 0;
}