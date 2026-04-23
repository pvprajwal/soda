#include <iostream>
#include <vector>
#include <chrono>
#include <math.h>

void profileCPU(int LIST_SIZE) {
    std::vector<float> A(LIST_SIZE, 1.0f);
    std::vector<float> B(LIST_SIZE, 2.0f);
    std::vector<float> C(LIST_SIZE);

    // Start Timer
    auto start = std::chrono::high_resolution_clock::now();

    // The CPU "Kernel" (Sequential)
    for (int i = 0; i < LIST_SIZE; i++) {
        C[i] = sin(A[i]) + sin(B[i]) + exp(A[i]);
    }

    // End Timer
    auto end = std::chrono::high_resolution_clock::now();
    
    std::chrono::duration<double, std::milli> duration = end - start;
    std::cout << "CPU Execution time (" << LIST_SIZE << "): " << duration.count() << " ms" << std::endl;
    std::cout << C[9] << std::endl;
}

int main(int argc, char* argv[])
{
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <LIST_SIZE>" << std::endl;
        return 1;
    }
    int LIST_SIZE = atoi(argv[1]);
    profileCPU(LIST_SIZE);
}