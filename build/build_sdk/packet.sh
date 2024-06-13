#!/bin/bash
# Copyright © Hisilicon Technologies. Co, Ltd. 2023-2023. All rights reserved.

set -e

SDK_BUILD_DIR=$(pwd)
SDK_SRC_DIR=$SDK_BUILD_DIR/../../src//non_real_time/drivers
OUTPUTDIR=${SDK_BUILD_DIR%/build_sdk}/output

source $SDK_BUILD_DIR/../build_exports

if [ ! -d "$OUTPUTDIR" ];then
    mkdir -p $OUTPUTDIR
fi

function mk_sdk()
{
    local sdk_file="hi3093sdk.tar.gz"
    local securec_file="securec.tar.gz"
    chmod 550 insmod_drv.sh
    chmod 640 Module.symvers

    tar -czvf ${sdk_file} ko insmod_drv.sh Module.symvers --format=gnu
    cp ${SDK_BUILD_DIR}/../../tempsymvers/securec/Module.symvers .
    cp -r ${SDK_BUILD_DIR}/../../platform/securec/include .
    tar -czvf ${securec_file} securec_ko Module.symvers include
    mv ${securec_file} ${OUTPUTDIR}
    mv ${sdk_file} ${OUTPUTDIR}
}

function clean_tmp_dir()
{
    rm -f Module.symvers
    rm -rf include
}

printf "\r\n\r\n\r\n################################### Begin to packet###################################\r\n"
mk_sdk
clean_tmp_dir
printf "\r\n\r\n\r\n################################### Finish to packet###################################\r\n"
