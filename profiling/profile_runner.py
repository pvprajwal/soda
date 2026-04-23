#!/usr/bin/env python3
import subprocess
import re
import csv
import os
from pathlib import Path

# Configuration
PROJECT_DIR = os.path.dirname(os.path.abspath(__file__))
CPU_PROFILER = os.path.join(PROJECT_DIR, "CPU_profiler.exe")
GPU_PROFILER = os.path.join(PROJECT_DIR, "GPU_profiler.exe")
OUTPUT_CSV = os.path.join(PROJECT_DIR, "profiling_results.csv")

# Generate list sizes with geometric progression for better step distribution
# Create sizes from 1024 to 1024000 with factor of 1.3, then add final size
LIST_SIZES = []
current = 1024
while current <= 1024000:
    LIST_SIZES.append(int(current))
    current *= 1.3
LIST_SIZES.append(1024000)  # Ensure 1024000 is included
LIST_SIZES.append(10240000)  # Add final large size
LIST_SIZES = sorted(list(set(LIST_SIZES)))  # Remove duplicates and sort

# Number of iterations per size for averaging
ITERATIONS = 7

def extract_time_from_output(output, pattern):
    """Extract execution time from program output using regex."""
    match = re.search(pattern, output)
    if match:
        return float(match.group(1))
    return None

def run_cpu_profiler(size):
    """Run CPU profiler and extract execution time."""
    try:
        result = subprocess.run(
            [CPU_PROFILER, str(size)],
            capture_output=True,
            text=True,
            timeout=120
        )
        # Pattern: "CPU Execution time (LIST_SIZE): X ms"
        cpu_time = extract_time_from_output(result.stdout, r"CPU Execution time.*?:\s*([\d.]+)\s*ms")
        return cpu_time
    except Exception as e:
        print(f"Error running CPU profiler for size {size}: {e}")
        return None

def run_gpu_profiler(size):
    """Run GPU profiler and extract GPU and total execution times."""
    try:
        result = subprocess.run(
            [GPU_PROFILER, str(size)],
            capture_output=True,
            text=True,
            timeout=120
        )
        # Pattern: "GPU Execution time: X ms"
        gpu_time = extract_time_from_output(result.stdout, r"GPU Execution time:\s*([\d.]+)\s*ms")
        # Pattern: "Total Execution time (LIST_SIZE): X ms"
        total_time = extract_time_from_output(result.stdout, r"Total Execution time.*?:\s*([\d.]+)\s*ms")
        return gpu_time, total_time
    except Exception as e:
        print(f"Error running GPU profiler for size {size}: {e}")
        return None, None

def average_results(values):
    """Calculate average of valid values."""
    valid_values = [v for v in values if v is not None]
    if not valid_values:
        return None
    return sum(valid_values) / len(valid_values)

def main():
    print("Starting profiling...")
    print(f"List sizes: {LIST_SIZES}")
    print(f"Iterations per size: {ITERATIONS}")
    print()
    
    results = []
    
    for size in LIST_SIZES:
        print(f"Profiling with LIST_SIZE={size}...")
        
        cpu_times = []
        gpu_times = []
        total_times = []
        
        for iteration in range(ITERATIONS):
            print(f"  Iteration {iteration + 1}/{ITERATIONS}...", end=" ", flush=True)
            
            # Run CPU profiler
            cpu_time = run_cpu_profiler(size)
            if cpu_time is not None:
                cpu_times.append(cpu_time)
                print(f"CPU={cpu_time:.2f}ms", end=" ", flush=True)
            
            # Run GPU profiler
            gpu_time, total_time = run_gpu_profiler(size)
            if gpu_time is not None:
                gpu_times.append(gpu_time)
                print(f"GPU={gpu_time:.2f}ms", end=" ", flush=True)
            if total_time is not None:
                total_times.append(total_time)
                print(f"Total={total_time:.2f}ms", end=" ", flush=True)
            
            print()
        
        # Calculate averages
        cpu_avg = average_results(cpu_times)
        gpu_avg = average_results(gpu_times)
        total_avg = average_results(total_times)
        
        results.append({
            'LIST_SIZE': size,
            'CPU_Time_ms': cpu_avg,
            'GPU_Time_ms': gpu_avg,
            'Total_GPU_Execution_ms': total_avg
        })
        
        print(f"  Averages - CPU: {cpu_avg:.2f}ms, GPU: {gpu_avg:.2f}ms, Total: {total_avg:.2f}ms")
        print()
    
    # Write to CSV
    print(f"Writing results to {OUTPUT_CSV}...")
    with open(OUTPUT_CSV, 'w', newline='') as csvfile:
        fieldnames = ['LIST_SIZE', 'CPU_Time_ms', 'GPU_Time_ms', 'Total_GPU_Execution_ms']
        writer = csv.DictWriter(csvfile, fieldnames=fieldnames)
        
        writer.writeheader()
        for row in results:
            writer.writerow(row)
    
    print(f"Profiling complete! Results saved to {OUTPUT_CSV}")

if __name__ == "__main__":
    main()
