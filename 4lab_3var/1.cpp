#include <sycl/sycl.hpp>
#include <iostream>
#include <vector>
#include <chrono>
#include <cmath>

void vectorScaleCPU(const std::vector<float>& A, std::vector<float>& B, float k) {
    for (size_t i = 0; i < A.size(); ++i) {
        B[i] = A[i] * k;
    }
}

int main() {
    const size_t N = 1000000;
    const float k = 2.5f;

    std::vector<float> h_A(N);
    std::vector<float> h_B_cpu(N, 0.0f);
    std::vector<float> h_B_gpu(N, 0.0f);

    for (size_t i = 0; i < N; ++i) {
        h_A[i] = static_cast<float>(i);
    }

    auto start_cpu = std::chrono::high_resolution_clock::now();
    vectorScaleCPU(h_A, h_B_cpu, k);
    auto end_cpu = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> time_cpu = end_cpu - start_cpu;

    try {
        sycl::queue q(sycl::default_selector_v);
        std::cout << "Task 1 running on: " << q.get_device().get_info<sycl::info::device::name>() << "\n";


        auto start_gpu = std::chrono::high_resolution_clock::now();

        {
        sycl::buffer<float, 1> bufA(h_A.data(), sycl::range<1>(N));
        sycl::buffer<float, 1> bufB(h_B_gpu.data(), sycl::range<1>(N));

        q.submit([&](sycl::handler& h) {
            auto accA = bufA.get_access<sycl::access::mode::read>(h);
            auto accB = bufB.get_access<sycl::access::mode::write>(h);

            h.parallel_for(sycl::range<1>(N), [=](sycl::id<1> idx) {
                accB[idx] = accA[idx] * k;
                });
            });

        q.wait();
        }

        auto end_gpu = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> time_gpu = end_gpu - start_gpu;

        bool match = true;
        for (size_t i = 0; i < N; ++i) {
            if (std::abs(h_B_cpu[i] - h_B_gpu[i]) > 1e-4f) {
                match = false;
                break;
            }
        }

        std::cout << "Result verification: " << (match ? "SUCCESS" : "FAILED") << "\n";
        std::cout << "CPU Execution Time: " << time_cpu.count() << " seconds\n";
        std::cout << "SYCL Execution Time: " << time_gpu.count() << " seconds\n";
        std::cout << "Speedup factor: " << time_cpu.count() / time_gpu.count() << "x\n";

    }
    catch (const sycl::exception& e) {
        std::cerr << "SYCL Exception caught: " << e.what() << "\n";
        return 1;
    }

    return 0;
}