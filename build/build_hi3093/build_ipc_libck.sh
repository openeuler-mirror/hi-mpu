#!/bin/bash
# Copyright © Hisilicon Technologies. Co, Ltd. 2023-2023. All rights reserved.

PWD=$(pwd)
ROOT_DIR=${PWD}/../..
BUILD_PATH=${PWD}

set -e
source ${ROOT_DIR}/build/build_exports

SAMPLE_OUTPUT_PATH=${BUILD_PATH}/output
LIBCK_BUILD=${BUILD_PATH}/../build_libck
SECUREC_PATH=${ROOT_DIR}/platform/securec
SMAPLE_IPC_PATH=${ROOT_DIR}/src/samples/non_real_time/ipc/libck
SAMPLE_IPC_LIBCK_PATH=${ROOT_DIR}/src/samples/real_time/libck/ipc
OPENAMP_OUTPUT_PATH=${BUILD_PATH}/output_amp/usr/lib64

source ${ROOT_DIR}/build/build_exports
export KERNEL_SRC=${KERNEL_PATH}

build_securec()
{
    if [ -d "${SAMPLE_OUTPUT_PATH}" ];then
        rm -rf ${SAMPLE_OUTPUT_PATH}
    fi
    mkdir -p ${SAMPLE_OUTPUT_PATH}

    cd ${LIBCK_BUILD}
    if [ -f "${LIBCK_BUILD}/build_libck_kernel.sh"];then
        ./build_libck_kernel.sh
    fi
    ./build_mcs.sh
    ./build_libck.sh

    if [ -d "${SECUREC_PATH}" ]; then
        cd ${SECUREC_PATH}/src

        if [ -f memset_s.c ]; then
            CROSS_CFLAGS=${CFLAGS}
            unset CFLAGS
            make CC=aarch64-openeuler-linux-gcc CFLAG_EXT=-fno-pic lib
            CFLAGS=${CROSS_CFLAGS}
        else
            cd ${SECUREC_PATH}/lib
        fi
        cp libsecurec.a  ${SMAPLE_IPC_PATH}
    fi
}

build_libck_mcs()
{
    export BUILD_SAMPLE_HI309X=oe_libck
    cd ${SAMPLE_IPC_LIBCK_PATH}
    make
    cp *.bin ${SAMPLE_OUTPUT_PATH} 
    make clean
}

build_sample_ipc()
{
    cp ${OPENAMP_OUTPUT_PATH}/libopen_amp.a  ${SMAPLE_IPC_PATH}

    cd ${SMAPLE_IPC_PATH}
    make
    cp ${SMAPLE_IPC_PATH}/rpmsg_libck_ipc ${SAMPLE_OUTPUT_PATH}
    make clean
}

build_securec
build_libck_mcs
build_sample_ipc
