cmake_minimum_required(VERSION 3.19)

project(openeuler_mcs_demo)

# append -pthread flags
add_compile_options(-pthread)

message(STATUS "DEMO_TARGET=${DEMO_TARGET}")

set(MCS_LIB mcs)

set(SHARED_LINK_LIBS
	libopen_amp.so
	libmetal.so
	libsysfs.so
)

add_subdirectory(library)

set(SRC_FILE
	./${DEMO_TARGET}/rpmsg_main.c
)

if(DEMO_TARGET MATCHES "openamp_demo")
	list(APPEND SRC_FILE "./rpmsg_pty_demo/rpmsg_pty.c")
	list(APPEND SRC_FILE "./${DEMO_TARGET}/rpmsg_matrix_multiply.c")
	list(APPEND SRC_FILE "./${DEMO_TARGET}/rpmsg_ping.c")
elseif(DEMO_TARGET MATCHES "rpmsg_pty_demo")
	list(APPEND SRC_FILE "./${DEMO_TARGET}/rpmsg_pty.c")
endif()

include_directories(
	${DEMO_TARGET}
	library/include/mcs
)

add_executable(rpmsg_main ${SRC_FILE})
target_link_libraries(rpmsg_main ${MCS_LIB} ${SHARED_LINK_LIBS})

install(TARGETS rpmsg_main DESTINATION bin)
