#!/bin/bash
# Copyright © Hisilicon Technologies. Co, Ltd. 2023-2023. All rights reserved.
PWD=$(pwd)
ROOT_DIR=${PWD}/../../

set -e

# build
OUTPUT_DIR=${ROOT_DIR}/build/build_baremetal/output/baremetal;
BAREMETAL_DIR=${ROOT_DIR}/src/real_time/baremetal;
REAL_TIME_DIR=${ROOT_DIR}/src/real_time;

source ${ROOT_DIR}/build/build_exports

mkdir -p ${OUTPUT_DIR}

export MCS_2WIHT2_HI309X=y

cd ${BAREMETAL_DIR}
cp common/hi309x_2with2.h include/hi309x_memmap.h
make
cp libbaremetal.a ${OUTPUT_DIR}
cp libbaremetal.a ${REAL_TIME_DIR}/lib/
make clean

tree ${OUTPUT_DIR} -L 3
