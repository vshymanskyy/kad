cmake_minimum_required (VERSION 2.6)

project (UDT)

add_definitions(-DUDT_EXPORTS)

file(GLOB_RECURSE udt_src ${PROJECT_SOURCE_DIR}/src/*)

add_library (udt SHARED ${udt_src})

if (WIN32)
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
	target_link_libraries(udt ws2_32)
endif (WIN32)
