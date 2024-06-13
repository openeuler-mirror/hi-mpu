#!/bin/bash
# Copyright © Hisilicon Technologies. Co, Ltd. 2023-2023. All rights reserved.

set -e

PROJECT_PATH=$(pwd)/../..
DIVIDE_BUILD_PATH=$PROJECT_PATH/tools/emmc_divide/
DIVIDE_FILE_NAME=emmc_divide
OUTPUTDIR=$PROJECT_PATH/build/build_sfc/sfc_output

if [ ! -d "sfc_output" ] ; then
    echo "mkdir sfc_output"
    mkdir sfc_output
fi

cd sfc_output
if [ ! -f "$DIVIDE_FILE_NAME" ] ; then
    echo "make emmc_divide"
    cd $DIVIDE_BUILD_PATH
    if [ ! -f "$DIVIDE_FILE_NAME" ] ; then
        make
    fi
    cp ./$DIVIDE_FILE_NAME $OUTPUTDIR
    make clean
fi
