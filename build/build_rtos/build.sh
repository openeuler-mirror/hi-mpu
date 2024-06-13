#!/bin/bash
# Copyright © Hisilicon Technologies. Co, Ltd. 2023-2023. All rights reserved.

set -e

RTOS_BUILD_DIR=$(pwd)
source $RTOS_BUILD_DIR/../build_exports

DTC=$RTOS_ARM64_PATH/bin/dtc
MKIMAGE=$RTOS_INSTALL_PATH/bin/modules/bin/mkheader
IMAGE_NAME=linux-
LOAD_ADDRESS=0x87480000
EP_ADDRESS=0x87480040

function get_image_form_rpm()
{
	cd $RTOS_ARM64_PATH/pool
	ar -x RTOS.dist
	IMAGE_NAME+=$(cat kernel-config | grep Linux/arm64 | awk '{print $3}')
	cp zImage $RTOS_BUILD_DIR
	cd -
	$MKIMAGE -A arm -O linux -T kernel -C none -a $LOAD_ADDRESS -e $EP_ADDRESS -n $IMAGE_NAME -d zImage uImage
	rm zImage
}

#install rtos_rpm & build_tools
prepare_build_env rtos

#Get Image
cd $RTOS_BUILD_DIR
get_image_form_rpm

#Build dtb
cd $RTOS_BUILD_DIR
$DTC -I dts -O dtb -o hi1711_asic.dtb hi1711_asic.dts
exit_if_error $? "DTS compiling error, please check DTS file!"
