cmake_minimum_required(VERSION 3.10)

project(GameOfLife)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenCL REQUIRED)

add_executable(app main.cpp)

target_link_libraries(app PRIVATE
    mingw32
    SDL2main
    SDL2
    OpenCL::OpenCL
)