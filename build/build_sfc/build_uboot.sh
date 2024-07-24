#!/bin/bash
# Copyright © Hisilicon Technologies. Co, Ltd. 2023-2023. All rights reserved.

set -e

PROJECT_PATH=$(pwd)/../..

source $PROJECT_PATH/build/version_${KERNEL_VERSION_MAIN}
source $PROJECT_PATH/build/build_exports

UBOOT_VER_PATH=u-boot
UBOOT_SRC_PATH=$PROJECT_PATH/open_source/u-boot
UBOOT_BUILD_PATH=$UBOOT_SRC_PATH/$UBOOT_VER_PATH
OUTPUTDIR=$PROJECT_PATH/build/build_sfc/sfc_output
UBOOT_INCLUDE_CONFIGS=$PROJECT_PATH/open_source/u-boot/u-boot/include/configs
BAREMETAL_INCLUDE=$PROJECT_PATH/src/real_time/baremetal/include
DOCKER_FILEDIR=$PROJECT_PATH/custom/open_euler


if [ ! -d "sfc_output" ] ; then
    echo "mkdir sfc_output"
    mkdir sfc_output
fi

function compile_uboot()
{
	cd $UBOOT_BUILD_PATH
	make mrproper
    set +e
    patch -N -p1 < $PROJECT_PATH/src/patches/uboot/uboot-mtd-spi.patch
    set -e
    if [ "$1" == "baremetal" ]; then
        cp $PROJECT_PATH/src/real_time/baremetal/common/hi309x_baremetal.h $UBOOT_INCLUDE_CONFIGS/hi309x_memmap.h
        make hi3093_sfc_baremetal_defconfig
	elif [ "$1" == "sfc_16M" ]; then
		set +e
		patch -R -p1 < $PROJECT_PATH/src/patches/uboot/uboot-mtd-spi.patch
		set -e
		make hi3093_sfc_16M_defconfig
	else
		make hi3093_sfc_defconfig
    fi
    make ARCH=arm CROSS_COMPILE=aarch64-openeuler-linux- all -j$COMPILE_PROCESS_NUM
    aarch64-openeuler-linux-objdump -D u-boot > u-boot.dump
}

function copy_uboot()
{
	cp $UBOOT_BUILD_PATH/u-boot.bin $OUTPUTDIR
	cp $UBOOT_BUILD_PATH/System.map $OUTPUTDIR
	cp $UBOOT_BUILD_PATH/u-boot.map $OUTPUTDIR
	cp $UBOOT_BUILD_PATH/u-boot.dump $OUTPUTDIR
}

function clean_uboot()
{
    if [ ! -d "$DOCKER_FILEDIR" ]; then
        mkdir -p $DOCKER_FILEDIR
    fi

    if [ -e "$DOCKER_FILEDIR"/mkimage ]; then
        rm $DOCKER_FILEDIR/mkimage
    fi

    cp $UBOOT_SRC_PATH/u-boot/tools/mkimage $DOCKER_FILEDIR
    cd $UBOOT_BUILD_PATH
    make ARCH=arm CROSS_COMPILE=aarch64-openeuler-linux- clean

    if [ -e "$UBOOT_BUILD_PATH"/scripts/dtc/dtc ]; then
        rm $UBOOT_BUILD_PATH/scripts/dtc/dtc
    fi

    if [ -e "$UBOOT_BUILD_PATH"/scripts/basic/fixdep ]; then
        rm $UBOOT_BUILD_PATH/scripts/basic/fixdep
    fi

    if [ -e "$UBOOT_BUILD_PATH"/scripts/kconfig/conf ]; then
        rm $UBOOT_BUILD_PATH/scripts/kconfig/conf
    fi
}

compile_uboot $1
copy_uboot
clean_uboot
