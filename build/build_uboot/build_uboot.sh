#!/bin/bash
# Copyright © Hisilicon Technologies. Co, Ltd. 2023-2023. All rights reserved.
PROJECT_PATH=$(pwd)/../..
UBOOT_BUILF_PATH=$PROJECT_PATH/build/build_uboot
ATF_BUILF_PATH=$PROJECT_PATH/build/build_atf
OUTPUTDIR=$PROJECT_PATH/build/output

set -e

if [ ! -d "$OUTPUTDIR" ];then
	mkdir -p $OUTPUTDIR
fi

cd $UBOOT_BUILF_PATH
./build.sh $1

cd $ATF_BUILF_PATH
./build_atf.sh

cd $UBOOT_BUILF_PATH
./packet.sh

cd $UBOOT_BUILF_PATH
./sign.sh
