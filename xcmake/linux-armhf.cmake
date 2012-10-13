# the name of the target operating system
SET(CMAKE_SYSTEM_NAME Linux)

#set mingw defaults
set(TOOLCHAIN_PREFIX   "arm-linux-gnueabihf-")
SET(CMAKE_C_COMPILER   ${TOOLCHAIN_PREFIX}gcc)
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PREFIX}g++-4.6)

# here is the target environment located
SET(CMAKE_FIND_ROOT_PATH /usr/arm-linux-gnueabihf)

# adjust the default behaviour of the FIND_XXX() commands:
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

