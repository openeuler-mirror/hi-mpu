#!/bin/bash
# Copyright © Hisilicon Technologies. Co, Ltd. 2023-2023. All rights reserved.

set -e

PROJECT_PATH=$(pwd)/../..
SFC_SDK_BUILD_PATH=$(pwd)
SFC_SDK_SRC_PATH=$PROJECT_PATH/src/non_real_time/drivers
SDIO_SFC_DIR=sdio_sfc

if [ ! -d "sfc_output" ] ; then
    echo "mkdir sfc_output"
    mkdir sfc_output
fi

cd $SFC_SDK_BUILD_PATH/sfc_output
if [ ! -d "ko" ] ; then
    echo "mkdir ko"
    mkdir ko
fi

cd $SFC_SDK_SRC_PATH
if [ ! -d "$sdio_emmc" ] ; then
    echo "copy sdio to sdio_emmc"
    cp -r ./sdio ./sdio_emmc
fi

if [ ! -d "$SDIO_SFC_DIR" ] ; then
    echo "patch for sfc"
    patch -p1 < $PROJECT_PATH/src/patches/openEuler/emmc_sfc.patch
    cp -rf ./sdio ./sdio_sfc
else
    echo "copy sdio_sfc to sdio"
    rm -rf ./sdio
    cp -rf ./sdio_sfc ./sdio
fi

cd $SFC_SDK_BUILD_PATH/../build_sdk
./build_sdk.sh
cp ./ko/* $SFC_SDK_BUILD_PATH/sfc_output/ko/
cp ./securec_ko/* $SFC_SDK_BUILD_PATH/sfc_output/ko/

cd $SFC_SDK_SRC_PATH
rm -rf ./sdio
mv ./sdio_emmc ./sdio
