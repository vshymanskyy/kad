cmake_minimum_required (VERSION 2.6)

project (UDT4)

add_definitions(-DUDT_EXPORTS)

file(GLOB_RECURSE udt4_src ${PROJECT_SOURCE_DIR}/src/*)

add_library (udt4 SHARED ${udt4_src})

if (WIN32)
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    target_link_libraries(udt4 ws2_32)
endif (WIN32)
