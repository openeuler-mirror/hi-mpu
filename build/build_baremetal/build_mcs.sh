#!/bin/bash
# Copyright © Hisilicon Technologies. Co, Ltd. 2023-2023. All rights reserved.
 
PWD=$(pwd)
ROOT_DIR=${PWD}/../..
BUILD_PATH=${PWD}

set -e

MCS_PATH=${ROOT_DIR}/open_source/mcs
LIBMETAL_PATH=${ROOT_DIR}/open_source/libmetal
OPENAMP_PATH=${ROOT_DIR}/open_source/open-amp
CROSS_COMPILE_GCC=aarch64-openeuler-linux-gcc

export KERNEL_SRC=${KERNEL_PATH}

build_libmetal()
{
    echo "######################### build metal #########################"
    cd ${BUILD_PATH}
 
    rm -rf output/libmetal
    mkdir -p output/libmetal/build
    cd output/libmetal/build
    cmake ${LIBMETAL_PATH} -DCMAKE_C_COMPILER=${CROSS_COMPILE_GCC} -DCMAKE_TOOLCHAIN_FILE=$LIBMETAL_PATH/cmake/platforms/baremetal_libmetal_arm64_gcc.cmake -DWITH_DOC=OFF -DWITH_EXAMPLES=OFF -DWITH_TESTS=OFF -DWITH_DEFAULT_LOGGER=OFF -DWITH_SHARED_LIB=OFF
    make VERBOSE=1 DESTDIR=../output install
    if [ $? -ne 0 ];then
        echo "make metal failed!"
        exit 1
    fi

    cd ..
    rm -rf build
    cp output/usr/local/* ./ -r
    rm -rf output
}
 
build_openamp()
{
    echo "######################### build openAMP #########################"
    cd ${BUILD_PATH}
    rm -rf output/openamp
    mkdir -p output/openamp/build
    cd output/openamp/build
    cmake ${OPENAMP_PATH} -DCMAKE_C_COMPILER=${CROSS_COMPILE_GCC} -DCMAKE_TOOLCHAIN_FILE=${OPENAMP_PATH}/cmake/platforms/baremetal_openamp_arm64_gcc.cmake -DDEMO_TARGET=baremetal
 
    make VERBOSE=1 DESTDIR=../output install
    if [ $? -ne 0 ];then
        echo "make open-amp failed!"
        exit 1
    fi

    cd ..
    rm -rf build
    cp output/usr/local/* ./ -r
    rm -rf output
}
 
build_mcs()
{
    echo "######################### build mcs #########################"
    cd ${MCS_PATH}/mcs_km
    make clean
    make
 
    cd ${MCS_PATH}
    rm -rf ${MCS_PATH}/build
    cmake -S . -B build -DCMAKE_C_FLAGS="-fstack-protector-strong -s" -DCMAKE_C_COMPILER=aarch64-openeuler-linux-gcc -DDEMO_TARGET=rpmsg_pty_baremetal
    cd ${MCS_PATH}/build
    make VERBOSE=1
 
    cd ${BUILD_PATH}
    rm -rf output/mcs_rpmsg
    mkdir -p output/mcs_rpmsg
    cp ${MCS_PATH}/mcs_km/mcs_km.ko output/mcs_rpmsg
    cp ${MCS_PATH}/build/rpmsg_main output/mcs_rpmsg
}
 
./build_depends.sh
build_libmetal
build_openamp
build_mcs
