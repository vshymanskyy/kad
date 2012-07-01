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
include_directories ("${PROJECT_SOURCE_DIR}/udt4/include")

include_directories ("${PROJECT_SOURCE_DIR}/src")
include_directories ("${PROJECT_SOURCE_DIR}/xplat/inc")

file(GLOB_RECURSE kad_src ${PROJECT_SOURCE_DIR}/src/*)
file(GLOB_RECURSE kad_tests ${PROJECT_SOURCE_DIR}/src/test/*.h)

add_custom_command (
	OUTPUT ${CMAKE_BINARY_DIR}/kad_test.cpp
	COMMAND python ${PROJECT_SOURCE_DIR}/cxxtest/bin/cxxtestgen --xunit-printer -o ${CMAKE_BINARY_DIR}/kad_test.cpp ${kad_tests}
)

add_subdirectory (udt4)
add_subdirectory (xplat)

add_library (kad ${kad_src})
target_link_libraries (kad xplat udt4)

add_executable(kad_test ${CMAKE_BINARY_DIR}/kad_test.cpp)
target_link_libraries (kad_test kad)

add_executable(kad_sim kad_sim.cpp)
target_link_libraries (kad_sim ${CMAKE_THREAD_LIBS_INIT} kad)
