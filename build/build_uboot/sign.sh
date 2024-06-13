#!/bin/bash
# Copyright © Hisilicon Technologies. Co, Ltd. 2023-2023. All rights reserved.

set -e

PWD=$(pwd)
PROJECT_PATH=${PWD}/../..
OUTPUTDIR=$PROJECT_PATH/build/output
SIGNDIR=$PROJECT_PATH/build/build_sign


function sign_uboot()
{
    if [ -e "$SIGNDIR"/u-boot.bin ]; then
        rm -rf $SIGNDIR/u-boot.bin
    fi

    cp -rf $OUTPUTDIR/u-boot.bin $SIGNDIR

    pushd $SIGNDIR
    if [ ! -f rsacert.cer ]; then
        echo hi3093 > rsacert.cer
    fi

    sh prepare_code_sign_data u-boot.bin
    sh generate_sign_image u-boot_rsa_4096.cfg
    popd

    if [ ! -e "$PROJECT_PATH"/build/build_sign/u-boot_rsa_4096.bin ]; then
        echo "make_uboot failed"
        exit 1
    fi

    cp -rf $SIGNDIR/u-boot_rsa_4096.bin $OUTPUTDIR/
}

source $PROJECT_PATH/build/build_exports
sign_uboot
