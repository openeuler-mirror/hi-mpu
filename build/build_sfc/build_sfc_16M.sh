#!/bin/bash
# Copyright © Hisilicon Technologies. Co, Ltd. 2023-2023. All rights reserved.

set -e

PROJECT_PATH=$(pwd)/../..
SFC_OUTPUT_PATH=$(pwd)/sfc_output
MOUNT_POINT=fs_mountpoint_15M
DOCKER_FILE_DIR=$PROJECT_PATH/custom/open_euler

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
./build_uboot.sh sfc_16M
#编译atf
echo "start build atf"
./build_atf.sh

#编译dtb
echo "start build dtb"
./build_dtb.sh
#编译内存文件系统
echo "start build initrd"
./build_initrd_8M.sh

#把uboot与atf打包
cd $SFC_OUTPUT_PATH
dd if=/dev/zero of=u-boot_new.bin bs=1024 count=512
dd if=u-boot.bin of=u-boot_new.bin bs=1024 count=440
dd if=bl31.bin of=u-boot_new.bin bs=1024 count=100 seek=440
mv u-boot.bin u-boot_only.bin
mv u-boot_new.bin u-boot.bin
chmod 777 *.bin

#生成内核镜像
pushd "${DOCKER_FILE_DIR}"
./mkimage -A arm -O linux -T kernel -C none -a 0x98280000 -e 0x98280040 -n linux-5.10.0 -d zImage-5.10.0 uImage
popd
#构建烧片包
cd $SFC_OUTPUT_PATH
dd if=/dev/zero of=ext4_15M.bin bs=1024 count=15872
mkfs.ext4 ext4_15M.bin
if [ ! -d "$MOUNT_POINT" ] ; then
    echo "mkdir fs_mountpoint_15M"
    mkdir $MOUNT_POINT
fi
sudo mount -t ext4 -o loop ext4_15M.bin $MOUNT_POINT
cd $MOUNT_POINT
sudo rm -rf lost+found
sudo mkdir boot
cd boot
#拷贝内核镜像
sudo cp $DOCKER_FILE_DIR/uImage .
#拷贝dtb
sudo cp $SFC_OUTPUT_PATH/hi1711_asic.dtb .
#拷贝内存文件系统
sudo cp $SFC_OUTPUT_PATH/initrd_boot_8M.cpio.gz ./initrd_boot.cpio.gz
cd $SFC_OUTPUT_PATH
sudo umount $MOUNT_POINT
#uboot
dd if=u-boot.bin of=Hi3093_flash_ext4.bin bs=1024 count=512
#ext4
dd if=ext4_15M.bin of=Hi3093_flash_ext4.bin bs=1024 count=15872 seek=512
