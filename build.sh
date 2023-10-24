#!/bin/bash

# Clear files function
clear_files() {
  rm cmake_install.cmake
  rm CMakeCache.txt
  rm -rf CMakeFiles
  rm Makefile
}

# Build
build() {
    cd build || exit 1
    cmake -DCMAKE_BUILD_TYPE=Release ..
    make
}


# Create options to clear or build with get opts
while getopts ":cb" opt; do
  case $opt in
    c)
      clear_files
      ;;
    b)
      build
      ;;
    \?)
      echo "Invalid option: -$OPTARG" >&2
      ;;
  esac
done
