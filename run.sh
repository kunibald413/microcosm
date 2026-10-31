#!/usr/bin/env bash
set -e  # stop on error

# Default to Debug, pass "Release" (or CMAKE_BUILD_TYPE value) as argument to match build config
CONFIG=${1:-Debug}

# Build via make.sh so we always run a fresh binary
./make.sh ${CONFIG}

./build/${CONFIG}/microcosm.exe
