#cross-compilation config
set(CMAKE_SYSTEM_PROCESSOR arm)
set(PROJECT_SYSTEM libck)
set(CMAKE_SYSTEM_NAME  libck)

set(CMAKE_C_COMPILER_WORKS 1)
set(CMAKE_CXX_COMPILER_WORKS 1)

set(CMAKE_C_FLAGS "-march=armv8-a -O2 -fstack-protector-strong -s -Wall -Wextra -std=gnu11 -fno-common -Wfloat-equal")
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER CACHE STRING "")
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY NEVER CACHE STRING "")
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE NEVER CACHE STRING "")
# vim: expandtab:ts=2:sw=2:smartindent
