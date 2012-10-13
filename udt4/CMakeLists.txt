cmake_minimum_required (VERSION 2.6)

project (UDT)

#set(CMAKE_CXX_FLAGS "-Wall -Wextra -finline-functions -O4 -fno-stack-protector -fno-strict-aliasing") #-msse3
add_definitions(-DUDT_EXPORTS)

file(GLOB_RECURSE udt_src "${PROJECT_SOURCE_DIR}/src/*")
include_directories("${PROJECT_SOURCE_DIR}/include/udt")

add_library (udt SHARED ${udt_src})

if (WIN32)
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
	target_link_libraries(udt ws2_32)
endif (WIN32)

