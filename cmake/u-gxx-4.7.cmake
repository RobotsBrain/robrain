# path to compiler


# specify the cross compiler
SET(CMAKE_C_COMPILER gcc-4.7)
SET(CMAKE_CXX_COMPILER g++-4.7)

# Name of the target platform
SET(CMAKE_SYSTEM_NAME Linux)

# Version of the system
SET(CMAKE_SYSTEM_VERSION 1)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)


set(OPENSOURCE_LIB_DIR u-gxx-4.7)
#add_compile_options(-Wall -Wno-unused-local-typedefs -O2 -Os -fomit-frame-pointer)