#!/bin/bash
# Copyright © Hisilicon Technologies. Co, Ltd. 2023-2023. All rights reserved.

set -e

ROOT_DIR=$(pwd)/../..
BUILD_PATH=$(pwd)
OUT_DIR=${BUILD_PATH}/output/libs
UNIPROTON_DRIVER_DIR=${ROOT_DIR}/src/samples/real_time/uniproton/driver
UNIPROTON_KERNEL_DIR=${ROOT_DIR}/src/real_time/uniproton/UniProton-v1.0.1
UNIPROTON_LIBS_DIR=${UNIPROTON_KERNEL_DIR}/demos/hi3093/libs/

build_driver()
{
    echo "######################### build driver #########################"
    rm -rf ${OUT_DIR}/*
    pushd ${UNIPROTON_DRIVER_DIR}
    make clean
    make
    if [ ! -d "$OUT_DIR" ]; then
        mkdir -p $OUT_DIR
    fi
    cp -rf libdriver.a ${OUT_DIR}
    make clean
    popd

    cp ${UNIPROTON_LIBS_DIR}/* ${OUT_DIR}
    rm -rf ${OUT_DIR}/.keep
}

build_driver