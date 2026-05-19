#include "MatrixProcessor.cpp"
#include <vector>
#include <iostream>

int main() {
    std::vector<std::vector<int>> mat = {
        {-1, 2, 3},
        {4, -5, 6},
        {7, -8, 9},
        {10, 11, -12}
    };

    MatrixProcessor<int> processor(mat, 3);

    processor.apply([](int x) { return x * 5; });

    processor.printMatrix();

    return 0;
}