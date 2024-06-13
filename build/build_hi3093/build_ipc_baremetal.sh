#!/bin/bash
# Copyright © Hisilicon Technologies. Co, Ltd. 2023-2023. All rights reserved.

PWD=$(pwd)
ROOT_DIR=${PWD}/../..
BUILD_PATH=${PWD}

set -e
# color
YELLOW="\e[33;1m"
DONE="\033[0m"
source ${ROOT_DIR}/build/build_exports

SAMPLE_OUTPUT_PATH=${BUILD_PATH}/output
BARENETAL_BUILD=${BUILD_PATH}/../build_baremetal
SMAPLE_IPC_PATH=${ROOT_DIR}/src/samples/non_real_time/ipc/baremetal
SAMPLE_REAL_TIME_BAREMETAL=${ROOT_DIR}/src/samples/real_time/baremetal/ipc
OPENAMP_OUTPUT_PATH=${BUILD_PATH}/output_amp/usr/lib64

source ${ROOT_DIR}/build/build_exports
export KERNEL_SRC=${KERNEL_PATH}

build_securec()
{
    if [ -d "${SAMPLE_OUTPUT_PATH}" ];then
        rm -rf ${SAMPLE_OUTPUT_PATH}
    fi
    mkdir -p ${SAMPLE_OUTPUT_PATH}

    cd ${BARENETAL_BUILD}
    ./build_mcs.sh
    ./build_baremetal_2with2.sh
    cp ${ROOT_DIR}/build/build_baremetal/output/baremetal/libsecurec.a ${SMAPLE_IPC_PATH}
}

build_baremetal_mcs()
{
    export BUILD_SAMPLE_HI309X="oe_bm"
    cd ${SAMPLE_REAL_TIME_BAREMETAL}
    make
    cp *.bin ${SAMPLE_OUTPUT_PATH} 
    make clean
}

build_sample_ipc()
{
    cp ${OPENAMP_OUTPUT_PATH}/libopen_amp.a  ${SMAPLE_IPC_PATH}

    cd ${SMAPLE_IPC_PATH}
    make
    cp ${SMAPLE_IPC_PATH}/rpmsg_baremetal_ipc ${SAMPLE_OUTPUT_PATH}
    make clean
}

build_securec
build_baremetal_mcs
build_sample_ipc
