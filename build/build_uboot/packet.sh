#!/bin/bash
# Copyright © Hisilicon Technologies. Co, Ltd. 2023-2023. All rights reserved.

PROJECT_PATH=$(pwd)/../..
OUTPUTDIR=$PROJECT_PATH/build/output

set -e

BLOCKSIZE=1024
UBOOT_CNT=440
ATF_CNT=64
UBOOT_BIN=u-boot.bin
ATF_BIN=bl31.bin

function append_atf()
{
    cd $OUTPUTDIR

    uboot_bin_size=$(stat --format=%s $UBOOT_BIN)
    if [ $uboot_bin_size -gt $(($UBOOT_CNT*$BLOCKSIZE)) ]; then
        echo "$UBOOT_BIN is overide, $uboot_bin_size, append_atf fail!"
        exit -1
    fi

    echo "atf: bs=$BLOCKSIZE, cnt=$ATF_CNT, seek=$UBOOT_CNT"
    dd if=$ATF_BIN of=$UBOOT_BIN bs=$BLOCKSIZE count=$ATF_CNT seek=$UBOOT_CNT

}

function packet()
{
	local uboot_file="uboot.tar.gz"
	
	tar -czvf ${uboot_file} u-boot* System.map bl31* --format=gnu
}

printf "\r\n\r\n\r\n################################### Begin to packet###################################\r\n"
append_atf
packet
printf "\r\n\r\n\r\n################################### Finish to packet###################################\r\n"
