cmake_minimum_required (VERSION 2.6)

project (Kademlia)
# The version number.
set (KAD_VERSION_MAJOR 0)
set (KAD_VERSION_MINOR 0)
set (KAD_VERSION_PATCH 1)

configure_file (
  "${PROJECT_SOURCE_DIR}/src/KadConfig.h.in"
  "${PROJECT_BINARY_DIR}/KadConfig.h"
  )

include_directories ("${PROJECT_BINARY_DIR}")

include_directories ("${PROJECT_SOURCE_DIR}/cxxtest")
include_directories ("${PROJECT_SOURCE_DIR}/src")
include_directories ("${PROJECT_SOURCE_DIR}/xplat/inc")

file(GLOB_RECURSE kad_src ${PROJECT_SOURCE_DIR}/src/*)
file(GLOB_RECURSE xplat_src ${PROJECT_SOURCE_DIR}/xplat/*)

file(GLOB_RECURSE kad_test ${PROJECT_SOURCE_DIR}/src/test/*.h)
file(GLOB_RECURSE xplat_test ${PROJECT_SOURCE_DIR}/xplat/test/*.h)

find_package (Threads)

add_custom_command (
  OUTPUT ${CMAKE_BINARY_DIR}/test.cpp
  COMMAND ${PROJECT_SOURCE_DIR}/cxxtest/bin/cxxtestgen -o ${CMAKE_BINARY_DIR}/test.cpp ${xplat_test} ${kad_test}
  )

add_executable(kad main.cpp ${CMAKE_BINARY_DIR}/test.cpp ${xplat_src} ${kad_src})
target_link_libraries (kad ${CMAKE_THREAD_LIBS_INIT} -ludt)

