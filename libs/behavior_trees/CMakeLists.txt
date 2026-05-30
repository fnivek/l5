cmake_minimum_required(VERSION 3.30)
project(behavior_trees)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(behavior_trees INTERFACE)
target_include_directories(behavior_trees INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

include(GNUInstallDirs)
install(TARGETS behavior_trees EXPORT behavior_trees-targets)
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

if(DOXYGEN_FOUND)
  set(DOXYGEN_PROJECT_NAME "behavior_trees")
  set(DOXYGEN_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build/docs/behavior_trees")
  set(DOXYGEN_GENERATE_HTML YES)
  set(DOXYGEN_GENERATE_LATEX NO)
  set(DOXYGEN_EXTRACT_ALL YES)
  if(DOXYGEN_DOT_FOUND)
    set(DOXYGEN_HAVE_DOT YES)
    set(DOXYGEN_CALL_GRAPH YES)
    set(DOXYGEN_CALLER_GRAPH YES)
  else()
    set(DOXYGEN_HAVE_DOT NO)
  endif()
  doxygen_add_docs(docs-behavior_trees
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    COMMENT "Generating behavior_trees HTML documentation"
  )
endif()
