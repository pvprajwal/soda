#define CL_TARGET_OPENCL_VERSION 300
#define CL_HPP_ENABLE_EXCEPTIONS
#include <CL/cl.h>
#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include "opencl.hpp"

// 1. THE KERNEL CODE (Running on GPU)
const char *kernelSource =
    "__kernel void Game_of_Life(__global int* A, __global int* B, int N) { "
    "   int i = get_global_id(0); "
    "   int x = i%N;"
    "   int y = i/N;"
    "   int neighbour_sum = 0;"

    // Check all 8 neighbors with boundary protection
    "   for(int dy = -1; dy <= 1; dy++) {"
    "       for(int dx = -1; dx <= 1; dx++) {"
    "           if(dx == 0 && dy == 0) continue;" // Skip the cell itself
    "           int nx = x + dx;"
    "           int ny = y + dy;"
    "           if(nx >= 0 && nx < N && ny >= 0 && ny < N) {"
    "               neighbour_sum += A[ny * N + nx];"
    "           }"
    "       }"
    "   }"

    "   if (neighbour_sum<2 || neighbour_sum>3){"
    "       B[i]=0;"
    "   }"
    "   else if(neighbour_sum==3){"
    "       B[i]=1;"
    "   }"
    "   else {"
    "       B[i]=A[i];"
    "   }"
    "} ";

void printGrid(std::vector<int> &arr, int N)
{
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            if (arr[i * N + j])
            {
                std::cout << "+ ";
            }
            else
            {
                std::cout << ". ";
            }
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

int main(void)
{
    int N = 10;
    int iter = 100;

    std::vector<int> curr_grid(N * N, 0);
    std::vector<int> next_grid(N * N, 0);
    std::vector<int> printing_grid(N * N, 0);

    // Add a "Blinker" pattern
    curr_grid[5 * N + 4] = 1;
    curr_grid[5 * N + 5] = 1;
    curr_grid[5 * N + 6] = 1;

    std::vector<cl::Platform> platforms;
    cl::Platform::get(&platforms);
    cl::Platform platform_main = platforms[0];

    std::vector<cl::Device> devices;
    platform_main.getDevices(CL_DEVICE_TYPE_GPU, &devices);
    cl::Device device_main = devices[0];

    // std::string name = device_main.getInfo<CL_DEVICE_NAME>();
    // std::cout << name << std::endl;

    cl::Context context(device_main);
    cl::CommandQueue queue(context, device_main);

    cl::Program program(context, kernelSource);
    try
    {
        program.build({device_main});
    }
    catch (cl::Error &e)
    {
        if (e.err() == CL_BUILD_PROGRAM_FAILURE)
        {
            std::string log = program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device_main);
            std::cerr << "Build Log:\n"
                      << log << std::endl;
        }
        throw e;
    }

    cl::Kernel kernel(program, "Game_of_Life");

    // --- SETUP (Outside Loop) ---
    cl::Buffer buf_A(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(int) * N * N, curr_grid.data());
    cl::Buffer buf_B(context, CL_MEM_READ_WRITE, sizeof(int) * N * N);

    cl::Buffer *input = &buf_A;
    cl::Buffer *output = &buf_B;

    // --- MAIN LOOP ---
    for (size_t i = 0; i < iter; i++)
    {
        auto frame_start = std::chrono::high_resolution_clock::now();

        // 1. Execute Kernel (Data stays on Intel Arc VRAM)
        kernel.setArg(0, *input);
        kernel.setArg(1, *output);
        kernel.setArg(2, N);
        queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(N * N), cl::NullRange);

        // 2. Ping-Pong: Swap pointers for the next round
        std::swap(input, output);

        // 3. Throttle and Display (e.g., 10Hz = 100ms)
        // Only read back when you actually want to print/draw
        queue.enqueueReadBuffer(*input, CL_TRUE, 0, sizeof(int) * N * N, printing_grid.data());
        printGrid(printing_grid, N);

        // Sleep to maintain frequency (don't let it run at 1000fps)
        std::this_thread::sleep_until(frame_start + std::chrono::milliseconds(100));
    }
}