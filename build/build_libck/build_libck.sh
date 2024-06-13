#!/bin/bash
# Copyright © Hisilicon Technologies. Co, Ltd. 2023-2023. All rights reserved.

PWD=$(pwd)
ROOT_DIR=${PWD}/../../

set -e

source ${ROOT_DIR}/build/build_exports

# build
OUTPUT_DIR_LIBCK=${ROOT_DIR}/build/build_libck/output/libck;
LIBCK_DIR=${ROOT_DIR}/src/real_time/libck;
REAL_TIME_DIR=${ROOT_DIR}/src/real_time;
LIBCK_KERNEL_DIR=${ROOT_DIR}/src/real_time/libck/kernel;

if [ ! -d "${REAL_TIME_DIR}/lib" ]; then
    mkdir -p ${REAL_TIME_DIR}/lib
fi

if [ ! -d "${ROOT_DIR}/build/build_libck/output/libck" ]; then
    mkdir -p ${ROOT_DIR}/build/build_libck/output/libck
fi

if [ ! -e "${REAL_TIME_DIR}/lib/libc.a" ] || [ ! -e "${REAL_TIME_DIR}/lib/libm.a" ]; then
    if [ -d "${ROOT_DIR}/open_source/musl" ]; then
        cd ${ROOT_DIR}/open_source/musl
        make clean
        ./configure --prefix=output CFLAGS=-fno-pic CROSS_COMPILE=aarch64-openeuler-linux- CC=aarch64-openeuler-linux-gcc ARCH=aarch64
        make -j32
        cp ${ROOT_DIR}/open_source/musl/lib/libc.a ${REAL_TIME_DIR}/lib/
        cp ${ROOT_DIR}/open_source/musl/lib/libm.a ${REAL_TIME_DIR}/lib/
        cp ${ROOT_DIR}/open_source/musl/lib/libc.a ${ROOT_DIR}/build/build_libck/output/libck/
        cp ${ROOT_DIR}/open_source/musl/lib/libm.a ${ROOT_DIR}/build/build_libck/output/libck/
        make clean
        rm -rf output
    fi
fi

cp ${LIBCK_KERNEL_DIR}/lib/libCortexMXsec_c.lib ${REAL_TIME_DIR}/lib/ -r

if [ -d "${OUTPUT_DIR_LIBCK}" ];then
    rm -rf ${OUTPUT_DIR_LIBCK}/liblibck.a
fi

mkdir -p ${OUTPUT_DIR_LIBCK}/lib
mkdir -p ${REAL_TIME_DIR}/lib/

cd ${LIBCK_DIR}
make clean
make
cp liblibck.a ${OUTPUT_DIR_LIBCK}/lib/
cp liblibck.a ${REAL_TIME_DIR}/lib/
make clean

tree ${OUTPUT_DIR_LIBCK} -L 3
