cmake_minimum_required(VERSION 3.10)

project(SODA)

set(CMAKE_CXX_STANDARD 17)

find_package(SDL2 REQUIRED)
find_package(OpenCL REQUIRED)

add_executable(app src/main.cpp)

file(COPY kernels DESTINATION ${CMAKE_BINARY_DIR})

target_include_directories(app PRIVATE ${SDL2_INCLUDE_DIRS})
target_link_libraries(app PRIVATE
    ${SDL2_LIBRARIES}
    OpenCL::OpenCL
)

if (WIN32)
    target_link_libraries(app PRIVATE mingw32 SDL2main)
endif()