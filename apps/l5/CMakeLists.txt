cmake_minimum_required(VERSION 3.30)
project(l5)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(l5 src/main.cpp)
target_link_libraries(l5 PRIVATE behavior_trees)

if(EMSCRIPTEN)
  # SDL1 via Emscripten's system port. Must be on both compile and link so the
  # port's include paths are injected at compile time.
  target_compile_options(l5 PRIVATE -sUSE_SDL=1)
  target_link_options(l5 PRIVATE -sUSE_SDL=1)

  set_target_properties(l5 PROPERTIES OUTPUT_NAME "index" SUFFIX ".html")
  install(FILES
      ${CMAKE_CURRENT_BINARY_DIR}/index.html
      ${CMAKE_CURRENT_BINARY_DIR}/index.js
      ${CMAKE_CURRENT_BINARY_DIR}/index.wasm
      DESTINATION share/l5)
else()
  # Native Linux build — SDL1 (via sdl12-compat in the Nix dev shell).
  find_package(SDL REQUIRED)
  target_include_directories(l5 PRIVATE ${SDL_INCLUDE_DIR})
  target_link_libraries(l5 PRIVATE ${SDL_LIBRARY})

  # Bake the compiler's implicit system includes into compile_commands.json so
  # clangd can find standard headers (stdio.h, libstdc++, glibc, …) without
  # needing the Nix shell environment to be active.
  #
  # Why not target_include_directories(SYSTEM …)?  CMake intentionally omits
  # SYSTEM dirs from compile_commands.json.  Using target_compile_options with
  # explicit -isystem flags forces them into the JSON where clangd can read them.
  foreach(dir IN LISTS CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES)
    target_compile_options(l5 PRIVATE "-isystem${dir}")
  endforeach()
endif()
