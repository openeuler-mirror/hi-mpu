#!/bin/bash
# Copyright © Hisilicon Technologies. Co, Ltd. 2023-2023. All rights reserved.

PWD=$(pwd)
ROOT_DIR=${PWD}/../..
BUILD_PATH=${PWD}

set -e

LIBMETAL_PATH=${ROOT_DIR}/open_source/libmetal
OPENAMP_PATH=${ROOT_DIR}/open_source/open-amp
TOOL_CHAIN_SYS_ROOT_DIR=/opt/buildtools/toolchains/aarch64-openeuler-linux/sysroot

build_libmetal()
{
    echo "######################### build metal #########################"
    cd ${LIBMETAL_PATH}
	
    if [ -d "build" ]; then
        rm -rf build
    fi

    mkdir build
    cd build
    cmake ../ -DCMAKE_SYSTEM_PROCESSOR=ARM -DCMAKE_FIND_ROOT_PATH=${SDKTARGETSYSROOT} -DCMAKE_TOOLCHAIN_FILE=../cmake/platforms/cross-openeuler-gcc.cmake -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_INSTALL_LIBDIR=/usr/lib64 -DWITH_DOC=OFF -DWITH_EXAMPLES=OFF -DWITH_TESTS=OFF -DWITH_STATIC_LIB=OFF

    # cp to output
    make VERBOSE=1 DESTDIR=${BUILD_PATH}/output_amp install

    make VERBOSE=1 DESTDIR=${SDKTARGETSYSROOT} install

    if [ $? -ne 0 ];then
        echo "make metal failed!"
        exit 1
    fi

    cd ..
    rm -rf build
}

build_openamp()
{
    echo "######################### build openAMP #########################"
	cd ${OPENAMP_PATH}
    if [ -d "build" ]; then
        rm -rf build
    fi

    mkdir build
    cd build
    cmake ../ -DCMAKE_SYSTEM_PROCESSOR=ARM -DCMAKE_TOOLCHAIN_FILE=../cmake/platforms/cross-openeuler-gcc.cmake -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_INSTALL_LIBDIR=/usr/lib64

    # cp to output
    make VERBOSE=1 DESTDIR=${BUILD_PATH}/output_amp install

    make VERBOSE=1 DESTDIR=${SDKTARGETSYSROOT} install

    if [ $? -ne 0 ];then
        echo "make open-amp failed!"
        exit 1
    fi

	cd ..
    rm -rf build
}

build_libmetal
build_openamp
