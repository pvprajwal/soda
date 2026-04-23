# Profiling Tool for CPU and GPU Vector Operations

This project contains profiling tools for comparing CPU and GPU performance on vector operations using OpenCL.

## Files

- `CPU_profiler.cpp`: C++ program to profile CPU execution time for vector operations.
- `GPU_profiler.cpp`: C++ program using OpenCL to profile GPU execution time and total execution time.
- `profile_runner.py`: Python script to automate running both profilers for various list sizes and collect results in a CSV file.
- `generate_visualizations.py`: Python script to generate visualizations from the profiling results, creating two PNG plots and a styled Excel file.

## Features

### CPU Profiler (`CPU_profiler.cpp`)

- Performs sequential vector operations: `C[i] = sin(A[i]) + sin(B[i]) + exp(A[i])`
- Measures execution time using C++ high-resolution chrono library.
- Takes `LIST_SIZE` as a command-line argument.
- Outputs the execution time in milliseconds and a sample result value.

### GPU Profiler (`GPU_profiler.cpp`)

- Uses OpenCL to execute the same vector operations on a GPU.
- Measures GPU kernel execution time using OpenCL profiling events.
- Measures total execution time including OpenCL setup, memory transfers, kernel execution, and data retrieval.
- Outputs both GPU time and total time separately.
- Requires an OpenCL-compatible GPU and runtime.

## How Time Measurement Works

### GPU Execution Time

The GPU execution time is measured using OpenCL's built-in profiling capabilities:

1. Profiling is enabled on the OpenCL command queue.
2. An event is associated with the kernel enqueue operation.
3. After the kernel completes, the start and end timestamps are retrieved from the event using `clGetEventProfilingInfo`.
4. The duration is calculated as `(time_end - time_start) / 1,000,000` to get milliseconds.

This measures only the time spent executing the kernel on the GPU, excluding setup and data transfer times.

### Total Execution Time

The total execution time includes all operations:

- OpenCL platform and device setup
- Context and command queue creation
- Memory buffer creation and host-to-device data transfer
- Kernel program compilation
- Kernel execution on GPU
- Device-to-host data transfer
- Cleanup

This is measured using C++ chrono from the start of the program (after parsing arguments) to the end, before cleanup.

## Usage

### Prerequisites

- C++ compiler (e.g., g++ or Visual Studio)
- OpenCL runtime and development headers
  - For NVIDIA GPUs: Install CUDA Toolkit (includes OpenCL)
  - For AMD GPUs: Install AMD APP SDK or ROCm
  - For Intel GPUs: Install Intel OpenCL SDK
  - On Windows, you may need to install the appropriate SDK for your GPU vendor
- Python 3 for running the automation script

### Installing OpenCL

1. **NVIDIA GPUs:**
   - Download and install CUDA Toolkit from NVIDIA's website
   - This includes OpenCL headers and runtime

2. **AMD GPUs:**
   - Download AMD APP SDK from AMD's developer website
   - Or use ROCm for newer AMD GPUs

3. **Intel GPUs:**
   - Download Intel OpenCL SDK from Intel's website

4. **Generic:**
   - On Windows, you can also try installing OpenCL from Microsoft's repository or use Khronos Group's OpenCL SDK

Ensure that OpenCL headers (`CL/cl.h`) and libraries are available to your compiler.

### Compiling the Programs

#### CPU Profiler

```bash
g++ CPU_profiler.cpp -o CPU_profiler.exe
```

#### GPU Profiler

```bash
g++ GPU_profiler.cpp -lOpenCL -o GPU_profiler.exe
```

Note: You may need to specify include paths and library paths if OpenCL is not in standard locations:

```bash
g++ GPU_profiler.cpp -I/path/to/opencl/includes -L/path/to/opencl/libs -lOpenCL -o GPU_profiler.exe
```

### Running the Profilers

#### Individual Runs

Run CPU profiler:
```bash
./CPU_profiler.exe <LIST_SIZE>
```

Run GPU profiler:
```bash
./GPU_profiler.exe <LIST_SIZE>
```

Example:
```bash
./CPU_profiler.exe 1024
./GPU_profiler.exe 1024
```

#### Automated Profiling

Use the Python script to run both profilers for multiple sizes and iterations:

```bash
python profile_runner.py
```

This will:
- Run profiling for list sizes from 1024 to 10,240,000
- Perform 7 iterations per size for averaging
- Generate `profiling_results.csv` with the results

### Generating Visualizations

After running the profiler:

```bash
python generate_visualizations.py
```

This will create:
- Two PNG plots comparing performance metrics
- A styled Excel file with the profiling results

#### Output Files:
- `performance_all_metrics.png`: Plot showing CPU time, GPU kernel time, and total GPU execution time
- `performance_cpu_vs_gpu.png`: Plot comparing CPU vs GPU kernel execution times
- `profiling_results_presentation_v3.xlsx`: Excel file with formatted profiling data (exports every other row plus the 1024000 size row)

## Output Format

### CPU Profiler Output
```
CPU Execution time (LIST_SIZE): X.XX ms
[sample result value]
```

### GPU Profiler Output
```
GPU Execution time: X.XX ms
Result of index 0: [value]
Total Execution time (LIST_SIZE): X.XX ms
```

### CSV Output (from profile_runner.py)
Columns: `LIST_SIZE`, `CPU_Time_ms`, `GPU_Time_ms`, `Total_GPU_Execution_ms`

## Notes

- The GPU profiler requires an OpenCL-compatible device
- Execution times may vary based on hardware and system load
- For accurate profiling, ensure no other GPU-intensive processes are running
- The vector operation is computationally intensive to show meaningful performance differences