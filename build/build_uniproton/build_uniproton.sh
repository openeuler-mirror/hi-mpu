#!/bin/bash
# Copyright © Hisilicon Technologies. Co, Ltd. 2023-2023. All rights reserved.

set -e

ROOT_DIR=$(pwd)/../..
BUILD_PATH=$(pwd)
OPENSOURCE_PATH=${ROOT_DIR}/open_source
MCS_PATH=${ROOT_DIR}/open_source/mcs
UNIPROTON_PATH=${ROOT_DIR}/src/real_time/uniproton
UNIPROTON_PATCH=${ROOT_DIR}/src/patches/uniproton
UNIPROTON_OUTPUT=${BUILD_PATH}/output
CROSS_COMPILE_GCC=aarch64-openeuler-linux-gcc

export KERNEL_SRC=${KERNEL_PATH}

prepare_env()
{
    if [ -d "${UNIPROTON_PATH}" ];then
        rm -rf ${UNIPROTON_PATH}
    fi
    mkdir -p ${UNIPROTON_PATH}
    echo "######################### get toolchains for cmake and gcc #########################"
    cd ${UNIPROTON_PATH}
    wget https://cmake.org/files/v3.20/cmake-3.20.5-linux-x86_64.tar.gz --no-check-certificate
    tar -zxvf cmake-3.20.5-linux-x86_64.tar.gz
    mv cmake-3.20.5-linux-x86_64 cmake-3.20.5
    rm cmake-3.20.5-linux-x86_64.tar.gz

    export PATH=${UNIPROTON_PATH}/cmake-3.20.5/bin:$PATH

    wget https://developer.arm.com/-/media/Files/downloads/gnu-a/10.3-2021.07/binrel/gcc-arm-10.3-2021.07-x86_64-aarch64-none-elf.tar.xz  --no-check-certificate
    tar -xvf gcc-arm-10.3-2021.07-x86_64-aarch64-none-elf.tar.xz
    rm gcc-arm-10.3-2021.07-x86_64-aarch64-none-elf.tar.xz
}

build_uniproton()
{
    echo "######################### build UniProton #########################"
    cd ${UNIPROTON_PATH}
    wget https://gitee.com/openeuler/UniProton/repository/archive/v1.0.1.zip --no-check-certificate
    unzip v1.0.1.zip
    rm -rf v1.0.1.zip

    cd ${UNIPROTON_PATH}/UniProton-v1.0.1
    patch -p1 < ${UNIPROTON_PATCH}/uniproton_update.patch
    cd ${UNIPROTON_PATH}/UniProton-v1.0.1/demos/hi3093/build
    sh ./build_app.sh

    cd ${BUILD_PATH}
    if [ -d "${UNIPROTON_OUTPUT}" ]; then
        rm -rf ${UNIPROTON_OUTPUT}/hi3093.bin
    fi

    mkdir -p output
    cp ${UNIPROTON_PATH}/UniProton-v1.0.1/demos/hi3093/build/hi3093.bin output
}

prepare_env
build_uniproton
./build_mcs.sh
