set (CROSS_PREFIX       "aarch64-openeuler-linux-" CACHE STRING "")
set (CMAKE_SYSTEM_NAME  "Linux"              CACHE STRING "")
set (CMAKE_C_COMPILER   "${CROSS_PREFIX}gcc" CACHE STRING "")
set (CMAKE_CXX_COMPILER "${CROSS_PREFIX}g++" CACHE STRING "")
set (CMAKE_C_FLAGS "-O2 -fstack-protector-strong -s -Wall -Wextra -std=gnu11 -fno-common -Wfloat-equal")
set (CMAKE_FIND_ROOT_PATH_MODE_PROGRAM BOTH CACHE STRING "")
set (CMAKE_FIND_ROOT_PATH_MODE_LIBRARY BOTH CACHE STRING "")
set (CMAKE_FIND_ROOT_PATH_MODE_INCLUDE BOTH CACHE STRING "")

# vim: expandtab:ts=2:sw=2:smartindent
