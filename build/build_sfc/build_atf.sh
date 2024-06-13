#!/bin/bash
# Copyright © Hisilicon Technologies. Co, Ltd. 2023-2023. All rights reserved.

set -e

PROJECT_PATH=$(pwd)/../..

source $PROJECT_PATH/build/build_exports

ATF_VER_PATH=arm-trusted-firmware-2.7
ATF_SRC_PATH=$PROJECT_PATH/open_source
ATF_BUILD_PATH=$ATF_SRC_PATH/$ATF_VER_PATH
ATF_PRIV_PATH=$PROJECT_PATH/open_source/arm-trusted-firmware-2.7/plat/hisilicon/hibmc/
ATF_OUT_PATH=$PROJECT_PATH/open_source/arm-trusted-firmware-2.7/build/Hi1711/release
OUTPUTDIR=$PROJECT_PATH/build/build_sfc/sfc_output

if [ ! -d "sfc_output" ] ; then
    echo "mkdir sfc_output"
    mkdir sfc_output
fi

pushd $ATF_BUILD_PATH
LINE_194_CONTENT=$(cat Makefile | sed -n '194p')
if [[ "${LINE_194_CONTENT}" != "LDFLAGS:=" ]];then
	sed -i '194i LDFLAGS:=' Makefile
fi
popd 

function compile_atf()
{
	cd $ATF_SRC_PATH
	cp $ATF_PRIV_PATH/* $ATF_VER_PATH/ -rf
	cd $ATF_BUILD_PATH
	
	make clean
	make all fip \
		 VERSION_MAJOR=013			\
		 VERSION_MINOR=				\
		 VERSION_SVN=69275			\
		 DEBUG=0                   \
		 UMPTE_BOARD=0				\
		 CHIP_VERIFY_BOARD=0			\
		 FVP_TSP_RAM_LOCATION=tdram     \
		 FVP_SHARED_DATA_LOCATION=tdram \
		 PLAT=Hi1711                  \
		 ARCH=aarch64					\
		 CROSS_COMPILE=aarch64-openeuler-linux-  \
		 TFCFG_COMPILE_PRODUCT=Hi1711	\
		-j$COMPILE_PROCESS_NUM
}

function copy_atf()
{
        cp $ATF_OUT_PATH/bl31.bin $OUTPUTDIR
        cp $ATF_OUT_PATH/bl31/bl31.dump $OUTPUTDIR
        cp $ATF_OUT_PATH/bl31/bl31.map $OUTPUTDIR
}
function clean()
{
    cd $ATF_BUILD_PATH
    make clean
    if [ -d "$ATF_OUT_PATH" ]; then
        rm -rf $ATF_OUT_PATH
    fi
}

compile_atf
copy_atf
clean
