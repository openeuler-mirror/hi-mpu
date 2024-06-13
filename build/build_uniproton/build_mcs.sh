#!/bin/bash
# Copyright © Hisilicon Technologies. Co, Ltd. 2023-2023. All rights reserved.
set -e

ROOT_DIR=$(pwd)/../..
BUILD_PATH=$(pwd)
MCS_PATH=${ROOT_DIR}/open_source/mcs
MCS_OUTPUT=${BUILD_PATH}/output
export KERNEL_SRC=${KERNEL_PATH}

build_mcs()
{
    cd ${BUILD_PATH}
    if [ ! -d "${MCS_OUTPUT}" ]; then
        mkdir -p ${MCS_OUTPUT}
    fi

    echo "######################### build mcs #########################"
    cd ${MCS_PATH}/mcs_km
    make clean
    make

    cd ${MCS_PATH}
    rm -rf ${MCS_PATH}/build
    cmake -S . -B build -DCMAKE_C_FLAGS="-fstack-protector-strong -s" -DDEMO_TARGET=rpmsg_pty_uniproton
    cd ${MCS_PATH}/build
    make VERBOSE=1
 
    cd ${BUILD_PATH}
    cp ${MCS_PATH}/mcs_km/mcs_km.ko output
    cp ${MCS_PATH}/build/rpmsg_main output
}

build_mcs
