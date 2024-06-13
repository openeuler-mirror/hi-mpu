cmake_minimum_required (VERSION 3.0.2)
if (POLICY CMP0048)
  cmake_policy(SET CMP0048 NEW)
endif()

if (POLICY CMP0077)
  cmake_policy(SET CMP0077 NEW)
endif()

if(DEMO_TARGET MATCHES "baremetal")
    set(BAREMETAL_PATH ../../src/real_time)
    include_directories(../../build/build_baremetal/output/libmetal/include lib/include)
    include_directories(${BAREMETAL_PATH}/baremetal/include)
    include_directories(${BAREMETAL_PATH}/baremetal/memory)
    include_directories(${BAREMETAL_PATH}/drivers/include)
elseif (DEMO_TARGET MATCHES "libck")
    set(LIBCK_PATH ../../src/real_time/libck)
    include_directories(../../build/build_libck/output/libmetal/include lib/include)
    include_directories(${LIBCK_PATH}/include)
    include_directories(${LIBCK_PATH}/kernel/include)
    include_directories(${LIBCK_PATH}/kernel/config)
    include_directories(${LIBCK_PATH}/bsp)
else()
    message("DEMO_TARGET: null")
endif()

set (OPENAMP_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set (OPENAMP_BIN_ROOT "${CMAKE_CURRENT_BINARY_DIR}")

list (APPEND CMAKE_MODULE_PATH
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake"
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules"
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/platforms")

include (syscheck)
project (open_amp C)

include (CheckIncludeFiles)
include (CheckCSourceCompiles)
include (collect)
include (options)
include (depends)
enable_testing ()


if (WITH_OBSOLETE)
  add_subdirectory (obsolete)
endif (WITH_OBSOLETE)

add_subdirectory (lib)

if (WITH_APPS)
  add_subdirectory (apps)
endif (WITH_APPS)
