#!/usr/bin/env bash
# Configures and builds native Linux targets with CMake + Make.
# Outputs to BUILD_DIR. Run from the repo root inside the Nix dev shell.
#
# Environment variables:
#   BUILD_DIR   Override the output directory (default: build/linux)
#   BUILD_TYPE  Override the build type (default: Debug)
set -euo pipefail

BUILD_DIR="${BUILD_DIR:-build/linux}"
BUILD_TYPE="${BUILD_TYPE:-Debug}"

mkdir -p "${BUILD_DIR}"
cmake -S . -B "${BUILD_DIR}" -DCMAKE_BUILD_TYPE="${BUILD_TYPE}"
make -C "${BUILD_DIR}" -j"$(nproc)"
