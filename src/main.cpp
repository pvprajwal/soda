#define CL_TARGET_OPENCL_VERSION 120
#define CL_HPP_TARGET_OPENCL_VERSION 120
#define CL_HPP_MINIMUM_OPENCL_VERSION 120
#define CL_SILENCE_DEPRECATION
#define CL_HPP_ENABLE_EXCEPTIONS

#include <iostream>
#include <thread>
#include <chrono>
#include <stdio.h>
#include <vector>
#include <fstream>
#include <sstream>

#include "../include/opencl.hpp"
#include <SDL2/SDL.h>

#define CELL_SIZE 6
#define CELL(x, y) host_grid[(y) * N + (x)]
#define N 100

std::string loadKernel(const std::string &path)
{
    std::ifstream file(path);
    if (!file.is_open())
    {
        std::cerr << "Failed to open kernel file\n";
        exit(1);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

int main(int argc, char *argv[])
{

    std::string kernelCode = loadKernel("kernels/ca_2d.cl");
    const char *kernelSource = kernelCode.c_str();

    std::vector<int> host_grid(N * N, 0);

    // initial pattern : choose pattern by uncommenting

    // // Still lives
    // // Block
    // CELL(10,10)=1; CELL(11,10)=1;
    // CELL(10,11)=1; CELL(11,11)=1;

    // //Loaf
    // CELL(20,10)=1; CELL(21,10)=1; CELL(22,10)=1;
    // CELL(19,11)=1; CELL(22,11)=1;
    // CELL(20,12)=1; CELL(22,12)=1;
    // CELL(21,13)=1;

    // // Osicallators
    // // Blinker
    // CELL(30,10)=1; CELL(31,10)=1; CELL(32,10)=1;

    // //Toad
    // CELL(40,10)=1; CELL(41,10)=1; CELL(42,10)=1;
    // CELL(39,11)=1; CELL(40,11)=1; CELL(41,11)=1;

    // // Beacon
    // CELL(50,10)=1; CELL(51,10)=1;
    // CELL(50,11)=1;

    // CELL(53,12)=1;
    // CELL(52,13)=1; CELL(53,13)=1;

    // // Spaceships
    // // Glider
    // CELL(10,30)=1;
    // CELL(11,31)=1;
    // CELL(9,32)=1; CELL(10,32)=1; CELL(11,32)=1;

    // Lightweight Spaceship (LWSS)
    // CELL(30,30)=1; CELL(33,30)=1;
    // CELL(34,31)=1;
    // CELL(30,32)=1; CELL(34,32)=1;
    // CELL(31,33)=1; CELL(32,33)=1; CELL(33,33)=1; CELL(34,33)=1;

    // Breeders
    // Glider Gun
    // left block
    CELL(1, 5) = 1;
    CELL(2, 5) = 1;
    CELL(1, 6) = 1;
    CELL(2, 6) = 1;

    // left structure
    CELL(11, 5) = 1;
    CELL(11, 6) = 1;
    CELL(11, 7) = 1;
    CELL(12, 4) = 1;
    CELL(12, 8) = 1;
    CELL(13, 3) = 1;
    CELL(13, 9) = 1;
    CELL(14, 3) = 1;
    CELL(14, 9) = 1;
    CELL(15, 6) = 1;
    CELL(16, 4) = 1;
    CELL(16, 8) = 1;
    CELL(17, 5) = 1;
    CELL(17, 6) = 1;
    CELL(17, 7) = 1;
    CELL(18, 6) = 1;

    // right block
    CELL(21, 3) = 1;
    CELL(21, 4) = 1;
    CELL(21, 5) = 1;
    CELL(22, 3) = 1;
    CELL(22, 4) = 1;
    CELL(22, 5) = 1;
    CELL(23, 2) = 1;
    CELL(23, 6) = 1;
    CELL(25, 1) = 1;
    CELL(25, 2) = 1;
    CELL(25, 6) = 1;
    CELL(25, 7) = 1;

    // far right block
    CELL(35, 3) = 1;
    CELL(35, 4) = 1;
    CELL(36, 3) = 1;
    CELL(36, 4) = 1;

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

    cl::Buffer buf_A(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
                     sizeof(int) * N * N, host_grid.data());

    cl::Buffer buf_B(context, CL_MEM_READ_WRITE,
                     sizeof(int) * N * N);

    cl::Buffer *input = &buf_A;
    cl::Buffer *output = &buf_B;

    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        printf("SDL Init Error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow(
        "SDL Test",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        800, 600,
        0);

    if (!window)
    {
        printf("Window Error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    int running = 1;
    SDL_Event event;

    while (running)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
                running = 0;
        }

        // Clear screen
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Draw grid (next step)
        for (int y = 0; y < N; y++)
        {
            for (int x = 0; x < N; x++)
            {

                if (host_grid[y * N + x] == 1)
                {
                    SDL_Rect cell = {
                        x * CELL_SIZE,
                        y * CELL_SIZE,
                        CELL_SIZE,
                        CELL_SIZE};

                    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                    SDL_RenderFillRect(renderer, &cell);
                }
            }
        }

        SDL_RenderPresent(renderer);

        // 1. Execute Kernel (Data stays on Intel Arc VRAM)
        kernel.setArg(0, *input);
        kernel.setArg(1, *output);
        kernel.setArg(2, N);
        queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(N * N), cl::NullRange);

        // 2. Ping-Pong: Swap pointers for the next round
        std::swap(input, output);

        // 3. Throttle and Display (e.g., 10Hz = 100ms)
        // Only read back when you actually want to print/draw
        queue.enqueueReadBuffer(*input, CL_TRUE, 0, sizeof(int) * N * N, host_grid.data());

        SDL_Delay(50);
    }

    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}