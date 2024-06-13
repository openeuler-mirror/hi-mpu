#!/bin/bash
# Copyright © Hisilicon Technologies. Co, Ltd. 2023-2023. All rights reserved.

set -e

PROJECT_PATH=$(pwd)/../..
INITRD_PATH=$(pwd)/initrd
DOCKER_FILE_DIR=$PROJECT_PATH/custom/open_euler

if [ ! -d "sfc_output" ] ; then
    echo "mkdir sfc_output"
    mkdir sfc_output
fi

if [ ! -d "$INITRD_PATH" ]; 
then
    echo "make initrd directory"
    mkdir initrd
fi
sudo rm -rf initrd/*
cp $DOCKER_FILE_DIR/*.rootfs.cpio.gz ./
cd initrd
zcat ../*.rootfs.cpio.gz | cpio -dim
#受限于sfc颗粒大小，对initrd进行裁剪
cd usr/bin
sudo rm gdb isulad strace
sudo cp $PROJECT_PATH/build/build_sfc/sfc_output/emmc_divide .
cd $INITRD_PATH
cd lib64
lib64_rmlist='libarchive.so.13*  libbfd-2.37.so  libdns.so.1115*  libgio-2.0.so.0*  libgnutls.so.30*  libisula_libutils.so  liblvm2cmd.so.2.03  liblxc.so.1*  libstdc++.so.6*  libtcl8.6.so.0  libunistring.so.5*  libxml2.so.2*'
for libbin in $lib64_rmlist
do 
        if [ ! -e "$libbin" ]
        then
            echo "$libbin: no this lib"
            continue
        fi
        rm $libbin
done
cd ..
sudo rm -rf lib/modules/5.10.0
#拷贝ko
cd lib
mkdir net
cp $PROJECT_PATH/build/build_sfc/sfc_output/ko/* net/
sudo chmod 777 -R net

cd $INITRD_PATH
sed -i "s/115200 ttyAMA0/115200 ttyS0 xterm/g" $(grep "115200 ttyAMA0" -rli ./)
sed -i "s/ttyAMA0:/ttyS0:/g" $(grep "ttyAMA0:" -rli ./)
cp $INITRD_PATH/../rc.sysinit $INITRD_PATH/etc/init.d/

cd $INITRD_PATH
find . | cpio -H newc -o | gzip -c > ../zfgemb.initrd
cd ..
ls -l zfgemb.initrd
mv zfgemb.initrd initrd_boot.cpio.gz
mv initrd_boot.cpio.gz $INITRD_PATH/../sfc_output/
echo "initrd make done"