#!/usr/bin/env bash
# Place this file next to src/ folder

# build
cmake -B build/     # Request that outputs from the build be placed in the build/ folder
cmake --build build/        # Start the build

# test
./build/solver_test
