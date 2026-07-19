cmake_minimum_required(VERSION 3.30)
project(l5)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ---------------------------------------------------------------------------
# Shared across both targets
# ---------------------------------------------------------------------------
set(L5_SHARED_SOURCES src/game_manager.cc)
set(L5_INCLUDE_DIRS   include)
set(L5_DEPS           behavior_trees Microsoft.GSL::GSL)

# ---------------------------------------------------------------------------
# Native target — skipped when cross-compiling with Emscripten
# ---------------------------------------------------------------------------
if(NOT EMSCRIPTEN)
  find_package(SDL3 REQUIRED CONFIG)

  add_executable(l5 src/main.cc ${L5_SHARED_SOURCES})
  target_include_directories(l5 PRIVATE ${L5_INCLUDE_DIRS})
  target_link_libraries(l5 PRIVATE ${L5_DEPS} SDL3::SDL3)

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

# ---------------------------------------------------------------------------
# Web target — only meaningful under emcmake
# ---------------------------------------------------------------------------
if(EMSCRIPTEN)
  set(L5_WEB_SDL_FLAGS -sUSE_SDL=3)

  add_executable(l5_web src/main_web.cc ${L5_SHARED_SOURCES})
  target_include_directories(l5_web PRIVATE ${L5_INCLUDE_DIRS})
  target_link_libraries(l5_web PRIVATE ${L5_DEPS})

  # SDL3 via Emscripten's system port. Must be on both compile and link so the
  # port's include paths are injected at compile time.
  target_compile_options(l5_web PRIVATE ${L5_WEB_SDL_FLAGS})
  target_link_options(l5_web PRIVATE
    ${L5_WEB_SDL_FLAGS}
    --preload-file ${CMAKE_SOURCE_DIR}/assets@assets
  )

  set_target_properties(l5_web PROPERTIES OUTPUT_NAME "index" SUFFIX ".html")
  install(FILES
      ${CMAKE_CURRENT_BINARY_DIR}/index.html
      ${CMAKE_CURRENT_BINARY_DIR}/index.js
      ${CMAKE_CURRENT_BINARY_DIR}/index.wasm
      ${CMAKE_CURRENT_BINARY_DIR}/index.data
      DESTINATION share/l5_web)
endif()

if(DOXYGEN_FOUND AND NOT EMSCRIPTEN)
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
