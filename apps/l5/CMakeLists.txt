cmake_minimum_required(VERSION 3.30)
project(l5)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(l5 src/main.cpp)
target_link_libraries(l5 PRIVATE behavior_trees)

set_target_properties(l5 PROPERTIES OUTPUT_NAME "index" SUFFIX ".html")
target_link_options(l5 PRIVATE -sUSE_SDL=1)
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/index.html
    ${CMAKE_CURRENT_BINARY_DIR}/index.js
    ${CMAKE_CURRENT_BINARY_DIR}/index.wasm
    DESTINATION share/l5)
