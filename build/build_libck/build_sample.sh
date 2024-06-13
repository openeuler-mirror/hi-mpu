#!/bin/bash
# Copyright © Hisilicon Technologies. Co, Ltd. 2023-2023. All rights reserved.

PWD=$(pwd)
ROOT_DIR=${PWD}/../../

# color
YELLOW="\e[33;1m"
DONE="\033[0m"

set -e

source ${ROOT_DIR}/build/build_exports

# build
cd ${ROOT_DIR}

SAMPLE_REAL_TIME_DIR=${ROOT_DIR}/src/samples/real_time
OUTPUT_DIR=${ROOT_DIR}/build/build_libck/output/;

sample_dirs_to_build=(
    mcs
    interrupt
    task_switch
    math
)

sample_driver_dirs_to_build=(
    gpio
    uart
    i2c
    net
    dmac
    spi
    timer
    watchdog
    localbus
)

function sample_build()
{
    if [ $# -lt 1 ]; then
        return
    fi

    for dir in "$@" ; do
        OUTPUT_SAMPLE_DIR=${ROOT_DIR}/build/build_libck/output/${dir};

        if [ -d "${OUTPUT_SAMPLE_DIR}" ];then
            rm -rf ${OUTPUT_SAMPLE_DIR}
        fi
        mkdir -p ${OUTPUT_SAMPLE_DIR}

        pushd ${dir}
        echo -e "${YELLOW}build ${dir}${DONE}"
        make clean
        make
        cp *.bin ${OUTPUT_SAMPLE_DIR}
        make clean
        popd
    done
}

echo -e "${YELLOW}build libck sample${DONE}"
export BUILD_SAMPLE_HI309X=oe_libck

echo "sample_dirs_to_build:${sample_dirs_to_build[@]}"
cd ${SAMPLE_REAL_TIME_DIR}/libck
sample_build ${sample_dirs_to_build[@]}

echo "sample_dirs_to_build:${sample_driver_dirs_to_build[@]}"
cd ${SAMPLE_REAL_TIME_DIR}/drivers
sample_build ${sample_driver_dirs_to_build[@]}

tree ${OUTPUT_DIR} -L 3
