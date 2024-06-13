#!/bin/bash
# Copyright © Hisilicon Technologies. Co, Ltd. 2023-2023. All rights reserved.

set -e

PROJECT_PATH=$(pwd)/../..
INITRD_PATH=$(pwd)/initrd
INITRD_8M_PATH=$(pwd)/initrd_8M
DOCKER_FILE_DIR=$PROJECT_PATH/custom/open_euler
OUTPUTDIR=$PROJECT_PATH/build/build_sfc/sfc_output

if [ ! -d "sfc_output" ] ; then
    echo "please run build_sfc.sh first!"
fi

if [ ! -d "$INITRD_8M_PATH" ]; 
then
    echo "make initrd_8M directory"
    mkdir initrd_8M
fi
sudo rm -rf initrd_8M/*
cp $OUTPUTDIR/initrd_boot.cpio.gz ./
cd initrd_8M
zcat ../initrd_boot.cpio.gz | cpio -dim
cd $INITRD_8M_PATH/../
#对initrd进行裁剪
function copy_ko()
{
    sudo rm -rf $INITRD_8M_PATH/lib/net/*
    sudo cp $OUTPUTDIR/ko/ksecurec.ko $INITRD_8M_PATH/lib/net/
    sudo cp $OUTPUTDIR/ko/log_drv.ko $INITRD_8M_PATH/lib/net/
    sudo cp $OUTPUTDIR/ko/comm_drv.ko $INITRD_8M_PATH/lib/net/
    sudo cp $OUTPUTDIR/ko/mdio_drv.ko $INITRD_8M_PATH/lib/net/
    sudo cp $OUTPUTDIR/ko/gmac_drv.ko $INITRD_8M_PATH/lib/net/
    sudo chmod 777 $INITRD_8M_PATH/lib/net/*
}

function change_bin()
{
    sudo rm $INITRD_8M_PATH/bin/watch.procps
    sudo rm $INITRD_8M_PATH/bin/*.coreutils
    cp $INITRD_PATH/usr/bin/scp.openssh $INITRD_8M_PATH/bin/
    cd $INITRD_8M_PATH/bin/
    ln -s scp.openssh scp
    cd ../../
}

function change_etc()
{
    sudo cp $INITRD_8M_PATH/../rc.sysinit8M $INITRD_8M_PATH/etc/init.d/rc.sysinit
    sudo rm $INITRD_8M_PATH/etc/rcS.d/S99run-postinsts
}

function change_lib64()
{
    sudo rm $INITRD_8M_PATH/lib64/libblkid.so*
    sudo rm $INITRD_8M_PATH/lib64/libext2fs.so*
    sudo rm $INITRD_8M_PATH/lib64/libfdisk.so*
    sudo rm $INITRD_8M_PATH/lib64/libmount.so*
    sudo rm $INITRD_8M_PATH/lib64/libncurses.so*
    sudo rm $INITRD_8M_PATH/lib64/libncursesw.so*
    sudo rm $INITRD_8M_PATH/lib64/libselinux.so*
    sudo rm $INITRD_8M_PATH/lib64/libsepol.so*
    sudo rm $INITRD_8M_PATH/lib64/libusb-1.0.so*
    sudo rm $INITRD_8M_PATH/lib64/libsmartcols*
    cp $INITRD_PATH/usr/lib64/libtirpc.so.3.0.0 $INITRD_8M_PATH/lib64/libtirpc.so.3
    cp $INITRD_PATH/usr/lib64/libcrypto.so.1.1 $INITRD_8M_PATH/lib64/
}

function change_sbin()
{
    sudo rm $INITRD_8M_PATH/sbin/e2fsck
    sudo rm $INITRD_8M_PATH/sbin/fsck.ext*
    sudo rm $INITRD_8M_PATH/sbin/ip.iproute2
    sudo rm $INITRD_8M_PATH/sbin/ldconfig
    sudo rm $INITRD_8M_PATH/sbin/load_policy
    sudo rm $INITRD_8M_PATH/sbin/semodule
    sudo rm $INITRD_8M_PATH/sbin/sestatus
}

function change_usr_bin()
{
    sudo rm -rf $INITRD_8M_PATH/usr/bin/*
    cp $INITRD_PATH/usr/bin/ssh-* $INITRD_8M_PATH/usr/bin/
    cp $INITRD_PATH/usr/bin/ssh.openssh $INITRD_8M_PATH/usr/bin/
    cd $INITRD_8M_PATH/usr/bin/
    ln -s ssh.openssh ssh
    cd $INITRD_8M_PATH/../
}

function change_usr_lib()
{
    sudo rm -rf $INITRD_8M_PATH/usr/lib/*
}

function change_usr_lib64()
{
    sudo rm -rf $INITRD_8M_PATH/usr/lib64/*
    cp $INITRD_PATH/usr/lib64/libpwquality* $INITRD_8M_PATH/usr/lib64/
}

function change_usr_sbin()
{
    sudo rm -rf $INITRD_8M_PATH/usr/sbin/*
}

function change_usr_share()
{
    sudo rm -rf $INITRD_8M_PATH/usr/share/gdb
    sudo rm -rf $INITRD_8M_PATH/usr/share/hwdata
    sudo rm -rf $INITRD_8M_PATH/usr/share/info
    sudo rm -rf $INITRD_8M_PATH/usr/share/lxc
    sudo rm -rf $INITRD_8M_PATH/usr/share/man
    sudo rm -rf $INITRD_8M_PATH/usr/share/misc
    sudo rm -rf $INITRD_8M_PATH/usr/share/screen
    sudo rm -rf $INITRD_8M_PATH/usr/share/tabset
    sudo rm -rf $INITRD_8M_PATH/usr/share/terminfo
    sudo rm -rf $INITRD_8M_PATH/usr/share/udhcpc
    sudo rm -rf $INITRD_8M_PATH/usr/share/zoneinfo
    sudo rm -rf $INITRD_8M_PATH/usr/share/pki
}

copy_ko
change_bin
change_etc
change_lib64
change_sbin
change_usr_bin
change_usr_lib
change_usr_lib64
change_usr_sbin
change_usr_share


cd $INITRD_8M_PATH
find . | cpio -H newc -o | gzip -c > ../zfgemb.initrd
cd ..
ls -lh zfgemb.initrd
mv zfgemb.initrd initrd_boot_8M.cpio.gz
mv initrd_boot_8M.cpio.gz $INITRD_8M_PATH/../sfc_output/
echo "initrd_8M make done"