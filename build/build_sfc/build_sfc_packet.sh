#!/bin/bash
# Copyright © Hisilicon Technologies. Co, Ltd. 2023-2023. All rights reserved.

set -e

PROJECT_PATH=$(pwd)/../..
SFC_OUTPUT_PATH=$(pwd)/sfc_output
MOUNT_POINT=fs_mountpoint
DOCKER_FILE_DIR=$PROJECT_PATH/custom/open_euler

if [ ! -d "sfc_output" ] ; then
    echo "mkdir sfc_output"
    mkdir sfc_output
fi

#把uboot与atf打包
cd $SFC_OUTPUT_PATH
dd if=/dev/zero of=u-boot_new.bin bs=1024 count=1024
dd if=u-boot.bin of=u-boot_new.bin bs=1024 count=540
dd if=bl31.bin of=u-boot_new.bin bs=1024 count=100 seek=540
mv u-boot.bin u-boot_only.bin
mv u-boot_new.bin u-boot.bin
chmod 777 *.bin

if [ ! -n "$1" ]; then
    #生成内容为0的ext4文件
    cd $SFC_OUTPUT_PATH
    dd if=/dev/zero of=ext4_46M.bin bs=1024 count=60k
    mkfs.ext4 ext4_46M.bin
    if [ ! -d "$MOUNT_POINT" ] ; then
        mkdir $MOUNT_POINT
    fi
    sudo rm -rf fs_mountpoint/* 
    sudo mount -t ext4 -o loop ext4_46M.bin fs_mountpoint/
    cd fs_mountpoint
    sudo mkdir boot
    cd boot
    #制作内核镜像
    pushd "${DOCKER_FILE_DIR}"
    ./mkimage -A arm -O linux -T kernel -C none -a 0x98280000 -e 0x98280040 -n linux-5.10.0 -d zImage-5.10.0 uImage
    popd

    if [ ! -e "$DOCKER_FILE_DIR"/uImage ]; then
        echo "make_uimage failed"
        exit 1
    fi  

    #拷贝内核镜像
    sudo mv $DOCKER_FILE_DIR/uImage .
    #拷贝dtb
    sudo cp $SFC_OUTPUT_PATH/hi1711_asic.dtb .
    #拷贝内存文件系统
    sudo cp $SFC_OUTPUT_PATH/initrd_boot.cpio.gz .

    cd ../../
    sudo umount fs_mountpoint
fi

#uboot0
dd if=u-boot.bin of=Hi3093_flash_ext4.bin bs=1024 count=2048

if [ ! -n "$1" ]; then
    #ext4fs
    dd if=ext4_46M.bin of=Hi3093_flash_ext4.bin bs=1024 count=60k seek=4096
fi
