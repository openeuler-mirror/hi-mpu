set(LIBCK_PATH ../../../../../src/real_time/libck)
include_directories(${LIBCK_PATH}/include)
include_directories(${LIBCK_PATH}/kernel/include)
include_directories(${LIBCK_PATH}/kernel/config)
include_directories(${LIBCK_PATH}/bsp)

collect (PROJECT_LIB_HEADERS alloc.h)
collect (PROJECT_LIB_HEADERS assert.h)
collect (PROJECT_LIB_HEADERS cache.h)
collect (PROJECT_LIB_HEADERS condition.h)
collect (PROJECT_LIB_HEADERS io.h)
collect (PROJECT_LIB_HEADERS irq.h)
collect (PROJECT_LIB_HEADERS log.h)
collect (PROJECT_LIB_HEADERS mutex.h)
collect (PROJECT_LIB_HEADERS sleep.h)
collect (PROJECT_LIB_HEADERS sys.h)

collect (PROJECT_LIB_SOURCES sys.c)
collect (PROJECT_LIB_SOURCES condition.c)
collect (PROJECT_LIB_SOURCES device.c)
collect (PROJECT_LIB_SOURCES init.c)
collect (PROJECT_LIB_SOURCES shmem.c)

# vim: expandtab:ts=2:sw=2:smartindent
