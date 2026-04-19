# Game of Life (OpenCL + SDL2)

A GPU-accelerated implementation of Conway's Game of Life using OpenCL for computation and SDL2 for visualization.

---

## Requirements

* C++17 compiler
* CMake (>= 3.10)
* MSYS2 (MinGW64 environment)
* SDL2 (installed via MSYS2)
* OpenCL runtime (Intel / NVIDIA / AMD drivers)

---

## Setup (Windows - MSYS2)

### 1. Install MSYS2

Download and install MSYS2 from:
https://www.msys2.org/

---

### 2. Open the correct terminal

Launch:

```
MSYS2 MinGW64
```

> ⚠️ Do NOT use the default MSYS shell

---

### 3. Install dependencies

Run the following inside the **MinGW64 terminal**:

```
pacman -S mingw-w64-x86_64-toolchain
pacman -S mingw-w64-x86_64-cmake
pacman -S mingw-w64-x86_64-SDL2
pacman -S mingw-w64-x86_64-opencl-headers
```

(Optional but recommended):

```
pacman -S mingw-w64-x86_64-ninja
```

---

### 4. Ensure OpenCL is available

Install GPU drivers or Intel oneAPI.
OpenCL runtime must be present on your system.

---

## Build

From the **MSYS2 MinGW64 terminal**:

```
cd /c/path/to/project
mkdir build
cd build

cmake -G "MinGW Makefiles" .. \
-DOpenCL_LIBRARY="C:/Program Files (x86)/Intel/oneAPI/compiler/latest/lib/OpenCL.lib" \
-DOpenCL_INCLUDE_DIR="C:/Program Files (x86)/Intel/oneAPI/compiler/latest/include"

cmake --build .
```

---

## Run

```
./app.exe
```

---

## Notes

* Always build from the **MinGW64 shell**
* Use Unix-style paths (`/c/...`) inside MSYS2
* If the app fails to start, ensure SDL runtime is available:

```
cp /mingw64/bin/SDL2.dll .
```

---

## Troubleshooting

### CMake cannot find OpenCL

Provide paths manually during configuration:

```
-DOpenCL_LIBRARY=...
-DOpenCL_INCLUDE_DIR=...
```

---

### CMake generator error

Ensure toolchain is installed:

```
pacman -S mingw-w64-x86_64-toolchain
```

---

### Build succeeds but no window appears

Check that:

* Grid is initialized with live cells
* SDL window is being rendered properly

---

## Project Structure

```
GameOfLife/
 ├── main.cpp
 ├── CMakeLists.txt
 ├── README.md
 └── build/
```
