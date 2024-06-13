#!/bin/bash
# Copyright © Hisilicon Technologies. Co, Ltd. 2023-2023. All rights reserved.

set -e

PROJECT_PATH=$(pwd)/../..
TOOLCHAIN_DIR=/home/${USER}/hi3093_tool/toolchain
source ${TOOLCHAIN_DIR}/environment-setup-aarch64-openeuler-linux

if [ ! -d "sfc_output" ] ; then
    echo "mkdir sfc_output"
    mkdir sfc_output
fi
cd $PROJECT_PATH/build
source $PROJECT_PATH/build/build_prepare.sh
source $PROJECT_PATH/build/build_exports
cd $PROJECT_PATH/build/build_sfc
#编译uboot
echo "start build uboot"
./build_uboot.sh $1
#编译atf
echo "start build atf"
./build_atf.sh
if [ ! -n "$1" ]; then
    #编译sdk，需要预先完成内核编译
    echo "start build sdk"
    ./build_sdk_sfc.sh
    #编译dtb
    echo "start build dtb"
    ./build_dtb.sh
    #编译tools
    echo "start build tools"
    ./build_tools.sh
    #编译内存文件系统
    echo "start build initrd"
    ./build_initrd.sh
fi

#构建烧片包
echo "start build packet"
./build_sfc_packet.sh $1
