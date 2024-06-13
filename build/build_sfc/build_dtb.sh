#!/bin/bash
# Copyright © Hisilicon Technologies. Co, Ltd. 2023-2023. All rights reserved.

set -e

PROJECT_PATH=$(pwd)/../..
DTC=$PROJECT_PATH/open_source/linux5.10/scripts/dtc/dtc
DTB_FILE_NAME=hi1711_asic.dtb
DTS_FILE_NAME=hi3093.dts

if [ ! -d "sfc_output" ] ; then
    echo "mkdir sfc_output"
    mkdir sfc_output
fi

if [ ! -f "$DTS_FILE_NAME" ] ; then
    echo "copy dts here"
    cp $PROJECT_PATH/build/build_rtos/$DTS_FILE_NAME .
fi

if [ -e "$DTB_FILE_NAME" ] ; then
    echo "delete dtb made before"
    rm $DTB_FILE_NAME
fi

$DTC -I dts -O dtb -o $DTB_FILE_NAME $DTS_FILE_NAME
mv $DTB_FILE_NAME  sfc_output/
rm $DTS_FILE_NAME
