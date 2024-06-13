#!/bin/bash
# Copyright © Hisilicon Technologies. Co, Ltd. 2023-2023. All rights reserved.

PWD=$(pwd)
ROOT_DIR=${PWD}/../../

set -e

# build
source ${ROOT_DIR}/build/build_exports

cd ${ROOT_DIR}
REAL_TIME_DIR=${ROOT_DIR}/src/real_time;
OUTPUT_DIR=${ROOT_DIR}/build/build_baremetal/output/baremetal;

mkdir -p ${OUTPUT_DIR}
mkdir -p ${REAL_TIME_DIR}/lib/

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
    cp libsecurec.a ${REAL_TIME_DIR}/lib/
    cp libsecurec.a ${ROOT_DIR}/build/build_baremetal/output/baremetal/
fi

if [ -d "${ROOT_DIR}/open_source/musl" ]; then
    cd ${ROOT_DIR}/open_source/musl
    make clean
    ./configure --prefix=output CFLAGS=-fno-pic CROSS_COMPILE=aarch64-openeuler-linux- CC=aarch64-openeuler-linux-gcc ARCH=aarch64
    make -j32
    cp ${ROOT_DIR}/open_source/musl/lib/libc.a ${REAL_TIME_DIR}/lib/
    cp ${ROOT_DIR}/open_source/musl/lib/libm.a ${REAL_TIME_DIR}/lib/
    cp ${ROOT_DIR}/open_source/musl/lib/libc.a ${ROOT_DIR}/build/build_baremetal/output/baremetal/
    cp ${ROOT_DIR}/open_source/musl/lib/libm.a ${ROOT_DIR}/build/build_baremetal/output/baremetal/
    make clean
    rm -rf output
fi
