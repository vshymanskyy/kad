# the name of the target operating system
SET(CMAKE_SYSTEM_NAME Windows)

#set mingw defaults
set(MINGW_PREFIX       "i586-mingw32msvc-")
SET(CMAKE_C_COMPILER   ${MINGW_PREFIX}gcc -D__MINGW32__)
set(CMAKE_CXX_COMPILER ${MINGW_PREFIX}g++ -D__MINGW32__)
set(CMAKE_RC_COMPILER  ${MINGW_PREFIX}windres)

# here is the target environment located
SET(CMAKE_FIND_ROOT_PATH /usr/i586-mingw32msvc/)

# adjust the default behaviour of the FIND_XXX() commands:
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
