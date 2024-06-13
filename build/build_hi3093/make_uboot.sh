#!/bin/bash
# Copyright © Hisilicon Technologies. Co, Ltd. 2023-2023. All rights reserved
set -e

PWD=$(pwd)
ROOT_DIR=${PWD}/../..
OUTPUTDIR_BUILD=${ROOT_DIR}/build/output
OUTPUTDIR_BM=${ROOT_DIR}/build/build_baremetal/output/uboot
OUTPUTDIR_HI3093=${ROOT_DIR}/build/build_hi3093/output
SYSTEM_TYPE=$1

make_uboot()
{
    echo "make_uboot begin"
    
    pushd ${ROOT_DIR}/build/build_uboot
    if [ "$SYSTEM_TYPE" == "bm" ]; then
        ./build_uboot.sh baremetal
    else
        ./build_uboot.sh
    fi
    popd

    if [ ! -e "$OUTPUTDIR_BUILD"/u-boot_rsa_4096.bin ]; then
        echo "make_uboot failed"
        exit 1
    fi

    if [ "$SYSTEM_TYPE" == "bm" ]; then
        if [ ! -d "$OUTPUTDIR_BM" ]; then
            mkdir -p $OUTPUTDIR_BM
        fi
        cp -rf $OUTPUTDIR_BUILD/u-boot_rsa_4096.bin $OUTPUTDIR_BM/
    else
        if [ ! -d "$OUTPUTDIR_HI3093" ]; then
            mkdir -p $OUTPUTDIR_HI3093
        fi
        cp -rf $OUTPUTDIR_BUILD/u-boot_rsa_4096.bin $OUTPUTDIR_HI3093/
    fi

    echo "make_uboot end"
}

make_uboot
