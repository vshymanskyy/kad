cmake_minimum_required (VERSION 2.6)

project (Kademlia)

set(CMAKE_BUILD_TYPE "Debug")
if (WIN32)
else (WIN32)
	set(CMAKE_CXX_FLAGS_DEBUG "-g3 -rdynamic -W -Wall -Wvla -Wextra -Wshadow -Winline -Wconversion -Wcast-align -Wno-long-long -Wwrite-strings -Wpointer-arith -Wredundant-decls -Wunsafe-loop-optimizations -Wno-variadic-macros -Wno-unused-parameter -Woverloaded-virtual -fstack-protector-all")
	set(CMAKE_CXX_FLAGS_RELEASE "-O4 -fno-stack-protector")
endif (WIN32)

# The version number.
set (KAD_VERSION_MAJOR 0)
set (KAD_VERSION_MINOR 1)
set (KAD_VERSION_PATCH 0)
 
configure_file (
  "${PROJECT_SOURCE_DIR}/src/KadConfig.h.in"
  "${PROJECT_BINARY_DIR}/KadConfig.h"
)

include_directories ("${PROJECT_BINARY_DIR}")
include_directories ("${PROJECT_SOURCE_DIR}/cxxtest")
include_directories ("${PROJECT_SOURCE_DIR}/src")
include_directories ("${PROJECT_SOURCE_DIR}/xplat/inc")

file(GLOB_RECURSE kad_src ${PROJECT_SOURCE_DIR}/src/*)
file(GLOB_RECURSE kad_tests ${PROJECT_SOURCE_DIR}/src/test/*.h)

add_custom_command (
	OUTPUT ${CMAKE_BINARY_DIR}/kad_test.cpp
	COMMAND python -B ${PROJECT_SOURCE_DIR}/cxxtest/bin/cxxtestgen --xunit-printer -o ${CMAKE_BINARY_DIR}/kad_test.cpp ${kad_tests}
)

add_subdirectory (xplat)

	add_subdirectory (udt4)
	include_directories ("${PROJECT_SOURCE_DIR}/udt4/include")
	
#if (WIN32)
	#link_directories("${PROJECT_SOURCE_DIR}/cryptopp/x64/Output/")
	#include_directories("${PROJECT_SOURCE_DIR}")
#endif (WIN32)

add_library (kad ${kad_src})
target_link_libraries (kad xplat udt msgpack cryptopp) # Win32: cryptlib

#add_executable(kad_test ${CMAKE_BINARY_DIR}/kad_test.cpp)
#target_link_libraries (kad_test kad)

#add_executable(kad_sim kad_sim.cpp)
#target_link_libraries (kad_sim kad)

add_executable(sign sign.cpp)
target_link_libraries (sign msgpack cryptopp) # Win32: cryptlib

add_executable(kad_node kad_node.cpp)
target_link_libraries (kad_node kad)
