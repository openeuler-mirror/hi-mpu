#!/bin/bash
# Copyright © Hisilicon Technologies. Co, Ltd. 2023-2023. All rights reserved.

set -e

PWD=$(pwd)
ROOT_DIR=${PWD}/../..
BUILD_PATH=${PWD}

SMAPLE_IPC_PATH=${ROOT_DIR}/src/samples/non_real_time/ipc/uniproton
SAMPLE_OUTPUT_PATH=${BUILD_PATH}/output
UNIPROTON_PATH=${ROOT_DIR}/src/real_time/uniproton
UNIPROTON_PATCH=${ROOT_DIR}/src/patches/uniproton
SECUREC_PATH=${ROOT_DIR}/platform/securec
SMAPLE_IPC_PATH=${ROOT_DIR}/src/samples/non_real_time/ipc/uniproton
OPENAMP_OUTPUT_PATH=${BUILD_PATH}/output_amp/usr/lib64

export KERNEL_SRC=${KERNEL_PATH}

build_securec()
{
    if [ -d "${SAMPLE_OUTPUT_PATH}" ];then
        rm -rf ${SAMPLE_OUTPUT_PATH}
    fi
    mkdir -p ${SAMPLE_OUTPUT_PATH}

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

build_uniproton_ipc()
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

    cp ${UNIPROTON_PATH}/UniProton-v1.0.1/demos/hi3093/build/hi3093.bin ${SAMPLE_OUTPUT_PATH}
}

build_sample_ipc()
{
    cp ${OPENAMP_OUTPUT_PATH}/libopen_amp.a  ${SMAPLE_IPC_PATH}

    cd ${SMAPLE_IPC_PATH}
    make
    cp ${SMAPLE_IPC_PATH}/rpmsg_uniproton_ipc ${SAMPLE_OUTPUT_PATH}
    make clean
}

build_securec
prepare_env
build_uniproton_ipc
build_sample_ipc
