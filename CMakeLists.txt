# CMake for a TCP server using boost library
cmake_minimum_required(VERSION 3.0)

project(tcp_server)

set(CMAKE_CXX_STANDARD 11)

set(SRC_DIR ${PROJECT_SOURCE_DIR}/src)
set(OUT_DIR ${PROJECT_SOURCE_DIR}/build)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OUT_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${OUT_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUT_DIR}/bin)

set(CMAKE_BUILD_TYPE Debug)

add_subdirectory(${SRC_DIR})
# add_executable(tcp_server main.cpp)