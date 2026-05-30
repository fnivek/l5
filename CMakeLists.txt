cmake_minimum_required(VERSION 3.30)
project(l5-monorepo)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Doxygen OPTIONAL_COMPONENTS dot)

add_subdirectory(libs/behavior_trees)
add_subdirectory(apps/l5)
