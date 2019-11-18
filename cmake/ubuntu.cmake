# path to compiler


# specify the cross compiler
SET(CMAKE_C_COMPILER gcc)
SET(CMAKE_CXX_COMPILER g++)

# Name of the target platform
SET(CMAKE_SYSTEM_NAME Linux)

# Version of the system
SET(CMAKE_SYSTEM_VERSION 1)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

set(OPENSOURCE_LIB_DIR ubuntu)

add_compile_options(-Wall -Wno-unused-parameter -Wno-unused-local-typedefs -O2 -Os -fomit-frame-pointer)
