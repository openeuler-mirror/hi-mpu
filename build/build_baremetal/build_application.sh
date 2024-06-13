#!/bin/bash
# Copyright © Hisilicon Technologies. Co, Ltd. 2023-2023. All rights reserved.
set -e

PWD=$(pwd)
ROOT_DIR=${PWD}/../..
BAREMETAL_DIR=${ROOT_DIR}/src/real_time/baremetal
SYSTEM_TYPE=$1

# color
YELLOW="\e[33;1m"
DONE="\033[0m"

if [ "${SYSTEM_TYPE}" == "-h" ] || [ "$#" -gt "1" ]; then
    echo -e "${YELLOW}./build_application.sh -h     : build help${DONE}"
    echo -e "${YELLOW}./build_application.sh bm     : build baremetal sample${DONE}"
    echo -e "${YELLOW}./build_application.sh oe_bm  : build 2+2 sample${DONE}"
    exit 1
fi

pushd $ROOT_DIR/build
set +e
source build_prepare.sh
set -e
popd

if [ "${SYSTEM_TYPE}" == "oe_bm" ]; then
    ./build_mcs.sh
    ./build_baremetal_2with2.sh
fi

if [ ! -n "${SYSTEM_TYPE}" ] || [ "${SYSTEM_TYPE}" == "bm" ]; then
    ./build_baremetal.sh
fi

./build_sample.sh ${SYSTEM_TYPE}
