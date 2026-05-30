@mainpage l5

# l5

**l5** is a cross-platform SDL3 game targeting native Linux and the browser via Emscripten/WebAssembly.

## Architecture

| Component | Description |
|-----------|-------------|
| `apps/l5` | Application entry point — SDL3 rendering loop and game logic |
| `libs/behavior_trees` | Header-only C++26 library for composable behavior trees |

## Source Layout

- `include/` — Public headers (e.g., `file_texture.h`)
- `src/main.cpp` — SDL3 initialization, event loop, and Emscripten integration

## Building

Native Linux (also used for clangd LSP):

```sh
task build-linux
```

WebAssembly (Emscripten):

```sh
task build-debug   # or task build-release
```
