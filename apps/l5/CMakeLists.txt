cmake_minimum_required(VERSION 3.30)
project(l5)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(l5 src/main.cpp)
target_link_libraries(l5 PRIVATE behavior_trees Microsoft.GSL::GSL)

if(EMSCRIPTEN)
  # SDL3 via Emscripten's system port. Must be on both compile and link so the
  # port's include paths are injected at compile time.
  target_compile_options(l5 PRIVATE -sUSE_SDL=3)
  target_link_options(l5 PRIVATE
    -sUSE_SDL=3
    --preload-file ${CMAKE_SOURCE_DIR}/assets@assets
  )

  set_target_properties(l5 PROPERTIES OUTPUT_NAME "index" SUFFIX ".html")
  install(FILES
      ${CMAKE_CURRENT_BINARY_DIR}/index.html
      ${CMAKE_CURRENT_BINARY_DIR}/index.js
      ${CMAKE_CURRENT_BINARY_DIR}/index.wasm
      ${CMAKE_CURRENT_BINARY_DIR}/index.data
      DESTINATION share/l5)
else()
  # Native Linux build — SDL3.
  find_package(SDL3 REQUIRED CONFIG)
  target_link_libraries(l5 PRIVATE SDL3::SDL3)

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

if(DOXYGEN_FOUND)
  set(DOXYGEN_PROJECT_NAME "l5")
  set(DOXYGEN_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build/docs/l5")
  set(DOXYGEN_GENERATE_HTML YES)
  set(DOXYGEN_GENERATE_LATEX NO)
  set(DOXYGEN_EXTRACT_ALL YES)
  set(DOXYGEN_USE_MDFILE_AS_MAINPAGE "${CMAKE_CURRENT_SOURCE_DIR}/docs/mainpage.md")
  if(DOXYGEN_DOT_FOUND)
    set(DOXYGEN_HAVE_DOT YES)
    set(DOXYGEN_CALL_GRAPH YES)
    set(DOXYGEN_CALLER_GRAPH YES)
  else()
    set(DOXYGEN_HAVE_DOT NO)
  endif()
  doxygen_add_docs(docs-l5
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
    "${CMAKE_CURRENT_SOURCE_DIR}/docs/mainpage.md"
    COMMENT "Generating l5 HTML documentation"
  )
endif()
