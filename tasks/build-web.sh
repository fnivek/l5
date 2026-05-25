#!/usr/bin/env bash
# Configures and builds Web (WASM) targets with Emscripten + CMake + Ninja.
# Outputs to BUILD_DIR. Run from the repo root inside the Nix dev shell.
#
# Environment variables:
#   BUILD_DIR   Override the output directory (default: build/debug)
#   BUILD_TYPE  Override the build type (default: Debug)
set -euo pipefail

BUILD_DIR="${BUILD_DIR:-build/debug}"
BUILD_TYPE="${BUILD_TYPE:-Debug}"

mkdir -p "${BUILD_DIR}"
emcmake cmake -S . -B "${BUILD_DIR}" -DCMAKE_BUILD_TYPE="${BUILD_TYPE}"
emmake make -C "${BUILD_DIR}" -j"$(nproc)"
