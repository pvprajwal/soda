#define CL_TARGET_OPENCL_VERSION 300
#include <CL/cl.h>
#include <iostream>
#include <vector>
#include <chrono>

// 1. THE KERNEL CODE (Running on GPU)
const char *kernelSource =
    "__kernel void vector_add(__global const float* A, __global const float* B, __global float* C) { "
    "   int i = get_global_id(0); "
    "   C[i] = sin(A[i]) + sin(B[i]) + exp(A[i]); "
    "} ";

int main(int argc, char* argv[])
{
    if (argc < 2) return 1;
    const int LIST_SIZE = atoi(argv[1]);
    std::vector<float> A(LIST_SIZE, 1.0f), B(LIST_SIZE, 2.0f), C(LIST_SIZE);

    auto start = std::chrono::high_resolution_clock::now();

    // 2. SETUP: Platform and Device
    cl_platform_id platform;
    cl_device_id device;
    clGetPlatformIDs(1, &platform, NULL);
    clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL);

    // 3. CREATE CONTEXT & QUEUE
    cl_context context = clCreateContext(NULL, 1, &device, NULL, NULL, NULL);
    // 3. CREATE QUEUE WITH PROFILING ENABLED
    cl_command_queue_properties props[] = {CL_QUEUE_PROPERTIES, CL_QUEUE_PROFILING_ENABLE, 0};
    cl_command_queue queue = clCreateCommandQueueWithProperties(context, device, props, NULL);

    // 4. CREATE GPU MEMORY BUFFERS
    cl_mem bufA = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(float) * LIST_SIZE, A.data(), NULL);
    cl_mem bufB = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(float) * LIST_SIZE, B.data(), NULL);
    cl_mem bufC = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(float) * LIST_SIZE, NULL, NULL);

    // 5. COMPILE THE KERNEL
    cl_program program = clCreateProgramWithSource(context, 1, &kernelSource, NULL, NULL);
    clBuildProgram(program, 1, &device, NULL, NULL, NULL);
    cl_kernel kernel = clCreateKernel(program, "vector_add", NULL);

    // 6. SET ARGUMENTS & EXECUTE
    clSetKernelArg(kernel, 0, sizeof(cl_mem), &bufA);
    clSetKernelArg(kernel, 1, sizeof(cl_mem), &bufB);
    clSetKernelArg(kernel, 2, sizeof(cl_mem), &bufC);

    size_t global_size = LIST_SIZE;
    cl_event event; // This is our "stopwatch"
    clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &global_size, NULL, 0, NULL, &event);

    // 7. WAIT AND CALCULATE TIME
    clWaitForEvents(1, &event); // Wait for GPU to finish

    cl_ulong time_start, time_end;
    clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_START, sizeof(time_start), &time_start, NULL);
    clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_END, sizeof(time_end), &time_end, NULL);

    double nanoSeconds = time_end - time_start;
    std::cout << "GPU Execution time: " << nanoSeconds / 1000000.0 << " ms" << std::endl;

    // 7. READ RESULTS BACK
    clEnqueueReadBuffer(queue, bufC, CL_TRUE, 0, sizeof(float) * LIST_SIZE, C.data(), 0, NULL, NULL);

    std::cout << "Result of index 0: " << C[0] << std::endl;
    auto end = std::chrono::high_resolution_clock::now();
    
    std::chrono::duration<double, std::milli> duration = end - start;
    std::cout << "Total Execution time (" << LIST_SIZE << "): " << duration.count() << " ms" << std::endl;

    // Cleanup
    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseMemObject(bufA);
    clReleaseMemObject(bufB);
    clReleaseMemObject(bufC);
    clReleaseCommandQueue(queue);
    clReleaseContext(context);

    return 0;
}