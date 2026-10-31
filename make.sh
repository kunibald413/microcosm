#!/usr/bin/env bash
set -e  # stop on error

# Default to Debug, pass "Release" (or CMAKE_BUILD_TYPE value) as argument to build Release
CONFIG=${1:-Debug}

cmake -S . -B build -G Ninja \
    -DCMAKE_BUILD_TYPE=${CONFIG} \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

cmake --build build
