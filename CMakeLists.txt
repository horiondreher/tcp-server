# CMake for a TCP server using boost library
cmake_minimum_required(VERSION 3.0)

project(tcp_server)

set(CMAKE_CXX_STANDARD 11)

# Separate directories for source and output
set(SRC_DIR ${PROJECT_SOURCE_DIR}/src)
set(CLIENT_DIR ${PROJECT_SOURCE_DIR}/client)
set(OUT_DIR ${PROJECT_SOURCE_DIR}/build)

# Set output directories
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OUT_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${OUT_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUT_DIR}/bin)

# Set build type
set(CMAKE_BUILD_TYPE Debug)

# Add src directory
add_subdirectory(${SRC_DIR})
add_subdirectory(${CLIENT_DIR})