cmake_minimum_required(VERSION 3.30)
project(l5)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(l5 src/main.cpp)
target_link_libraries(l5 PRIVATE behavior_trees)

include(GNUInstallDirs)
install(TARGETS l5 RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
