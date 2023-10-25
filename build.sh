#!/bin/bash

CLEAR=false
BUILD=false

# Create options to clear or build with get opts
while getopts ":cb" opt; do
  case $opt in
    c)
      CLEAR=true
      ;;
    b)
      BUILD=true
      ;;
    \?)
      echo "Invalid option: -$OPTARG" >&2
      ;;
  esac
done

# Clear files if option is set
if [ "$CLEAR" = true ] ; then
  rm -rf build/*
  rm -rf output/*
fi

# Build if option is set
if [ "$BUILD" = true ] ; then
  if [ ! -d "build" ]; then
    mkdir build
  fi

  if [ ! -d "output" ]; then
    mkdir output
  fi

  cd build || exit 1
  cmake -DCMAKE_BUILD_TYPE=Release ..
  make
fi
