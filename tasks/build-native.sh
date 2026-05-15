#!/usr/bin/env bash
# Configures and builds all C++ targets natively using CMake + Ninja.
# Outputs to build/. Run from the repo root inside the Nix dev shell.
#
# Environment variables:
#   BUILD_DIR  Override the output directory (default: build)
#   BUILD_TYPE  Override the build type (default: Release)
set -euo pipefail

BUILD_DIR="${BUILD_DIR:-build}"
BUILD_TYPE="${BUILD_TYPE:-Release}"

mkdir -p "${BUILD_DIR}"
cmake -S . -B "${BUILD_DIR}" -GNinja -DCMAKE_BUILD_TYPE="${BUILD_TYPE}"
ninja -C "${BUILD_DIR}"
