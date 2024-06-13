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
OUTPUT_DIR=${ROOT_DIR}/build/build_baremetal/output/;

sample_dirs_to_build=(
    hwi
    jitter
    message
    irq_perf
    math
)

sample_2with2_dirs_to_build=(
    hwi
    jitter
    message
    mcs
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
        OUTPUT_SAMPLE_DIR=${ROOT_DIR}/build/build_baremetal/output/${dir};
        if [ -d "${OUTPUT_SAMPLE_DIR}" ];then
            rm -rf ${OUTPUT_SAMPLE_DIR}
        fi

        mkdir -p ${OUTPUT_SAMPLE_DIR}

        pushd ${dir}
        echo -e "${YELLOW}build ${dir}${DONE}"
        make
        cp *.bin ${OUTPUT_SAMPLE_DIR}
        make clean
        popd
    done
}

if [ "$1" == "-h" ] || [ "$#" -gt "1" ]; then
    echo -e "${YELLOW}./build_sample.sh -h     : build help${DONE}"
    echo -e "${YELLOW}./build_sample.sh bm     : build baremetal sample${DONE}"
    echo -e "${YELLOW}./build_sample.sh oe_bm  : build 2+2 sample${DONE}"
    exit 1
fi

if [ ! -n "$1" ] || [ "$1" == "bm" ]; then
    echo -e "${YELLOW}build baremetal sample${DONE}"
    export BUILD_SAMPLE_HI309X="bm"
    echo "sample_dirs_to_build:${sample_dirs_to_build[@]}"
    cd ${SAMPLE_REAL_TIME_DIR}/baremetal
    sample_build ${sample_dirs_to_build[@]}

    echo "sample_dirs_to_build:${sample_driver_dirs_to_build[@]}"
    cd ${SAMPLE_REAL_TIME_DIR}/drivers
    sample_build ${sample_driver_dirs_to_build[@]}
fi

if [ "$1" == "oe_bm" ]; then
    echo -e "${YELLOW}build baremetal 2+2 sample${DONE}"
    export BUILD_SAMPLE_HI309X="oe_bm"
    echo "sample_dirs_to_build:${sample_2with2_dirs_to_build[@]}"
    cd ${SAMPLE_REAL_TIME_DIR}/baremetal
    sample_build ${sample_2with2_dirs_to_build[@]}

    echo "sample_dirs_to_build:${sample_driver_dirs_to_build[@]}"
    cd ${SAMPLE_REAL_TIME_DIR}/drivers
    sample_build ${sample_driver_dirs_to_build[@]}
fi

tree ${OUTPUT_DIR} -L 3
