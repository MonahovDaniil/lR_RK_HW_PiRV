#include <sycl/sycl.hpp>
#include <iostream>
#include <vector>
#include <chrono>

void thresholdCPU(const std::vector<unsigned char>& in, std::vector<unsigned char>& out, int width, int height, unsigned char T) {
    for (int r = 0; r < height; ++r) {
        for (int c = 0; c < width; ++c) {
            int idx = r * width + c;
            out[idx] = (in[idx] > T) ? 255 : 0;
        }
    }
}

int main() {
    const int width = 1024;
    const int height = 1024;
    const size_t total_pixels = width * height;
    const unsigned char T = 128;

    std::vector<unsigned char> h_in(total_pixels);
    std::vector<unsigned char> h_out_cpu(total_pixels, 0);
    std::vector<unsigned char> h_out_gpu(total_pixels, 0);

    for (size_t i = 0; i < total_pixels; ++i) {
        h_in[i] = static_cast<unsigned char>(i % 256);
    }

    auto start_cpu = std::chrono::high_resolution_clock::now();
    thresholdCPU(h_in, h_out_cpu, width, height, T);
    auto end_cpu = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> time_cpu = end_cpu - start_cpu;

    try {
        sycl::queue q(sycl::default_selector_v);
        std::cout << "Task 2 running on: " << q.get_device().get_info<sycl::info::device::name>() << "\n";

        auto start_gpu = std::chrono::high_resolution_clock::now();

        {
        sycl::buffer<unsigned char, 2> bufIn(h_in.data(), sycl::range<2>(height, width));
        sycl::buffer<unsigned char, 2> bufOut(h_out_gpu.data(), sycl::range<2>(height, width));

        q.submit([&](sycl::handler& h) {
            auto accIn = bufIn.get_access<sycl::access::mode::read>(h);
            auto accOut = bufOut.get_access<sycl::access::mode::write>(h);

            sycl::range<2> global_size(height, width);
            sycl::range<2> local_size(16, 16);
            sycl::nd_range<2> execution_range(global_size, local_size);

            h.parallel_for(execution_range, [=](sycl::nd_item<2> item) {
                sycl::id<2> global_id = item.get_global_id();

                if (accIn[global_id] > T) {
                    accOut[global_id] = 255;
                }
                else {
                    accOut[global_id] = 0;
                }
                });
            });

        q.wait();
        }

        auto end_gpu = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> time_gpu = end_gpu - start_gpu;

        bool match = true;
        for (size_t i = 0; i < total_pixels; ++i) {
            if (h_out_cpu[i] != h_out_gpu[i]) {
                match = false;
                break;
            }
        }

        std::cout << "Result verification: " << (match ? "SUCCESS" : "FAILED") << "\n";
        std::cout << "CPU Execution Time: " << time_cpu.count() << " seconds\n";
        std::cout << "SYCL 2D-ND-range Time: " << time_gpu.count() << " seconds\n";
        std::cout << "Acceleration factor: " << time_cpu.count() / time_gpu.count() << "x\n";

    }
    catch (const sycl::exception& e) {
        std::cerr << "SYCL Exception caught: " << e.what() << "\n";
        return 1;
    }

    return 0;
}