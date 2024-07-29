#!/bin/bash
# Copyright © Hisilicon Technologies. Co, Ltd. 2023-2023. All rights reserved.
#packet hi3093

set -e

echo "begin"
WORKING_DIR=$(pwd)
OUTPUTDIR=$WORKING_DIR/../output

echo $WORKING_DIR
echo $OUTPUTDIR
SYS_TYPE=$1
SDK_PACKET_FILE=$OUTPUTDIR/hi3093sdk.tar.gz
CPIO_PACKER_FILE=$OUTPUTDIR/initrd_boot.cpio.gz
SECUREC_PACKER_FILE=$OUTPUTDIR/securec.tar.gz
UIMAGE_FILE=$OUTPUTDIR/uImage
DTB_FILE=$WORKING_DIR/../build_rtos/hi1711_asic.dtb
EXT4CMS_FILE=$WORKING_DIR/../build_sign/Hi3093_ext4fs_cms.bin
TARGET_FILE=hi3093_ext4.tar.gz
EXT4_TARGET_BIN=$WORKING_DIR/Hi3093_ext4fs.img

SDK_UNPACKET_DIR=$WORKING_DIR/hi3093sdk
CPIO_UNPACKER_DIR=$WORKING_DIR/cpio_dir
TEMP_CPIO_UNPACKER_DIR=$WORKING_DIR/tmp_cpio_dir
FINAL_CPIO_DIR=$WORKING_DIR/cpio_fs
EXT4CMS_DIR=$WORKING_DIR/../build_sign
BAREMETAL_OUTPUT_DIR=$WORKING_DIR/../build_baremetal/output
UNIPROTON_OUTPUT_DIR=${WORKING_DIR}/../build_uniproton/output
LIBCK_OUTPUT_DIR=${WORKING_DIR}/../build_libck/output
OPEN_AMP_OUTPUT_DIR=${WORKING_DIR}/../build_hi3093/output_amp
CUSTOM_DIR=$WORKING_DIR/../../custom

packet_check()
{
	cd $WORKING_DIR
	echo "packet_check begin"
	# 1 check sdk is exist?
	if [ ! -e "$SDK_PACKET_FILE" ]; then
		echo "$SDK_PACKET_FILE isn't exist"
		exit 1
	fi

	if [ ! -e "$SECUREC_PACKER_FILE" ]; then
		echo "$SECUREC_PACKER_FILE isn't exist"
		exit 1
	fi

	# 2 check cpio is exist?
	if [ ! -e "$CPIO_PACKER_FILE" ]; then
		echo "$CPIO_PACKER_FILE isn't exist"
		exit 1
	fi

	# 3 check uImage is exist?
	if [ ! -e "$UIMAGE_FILE" ]; then
		echo "$UIMAGE_FILE isn't exist"
		exit 1
	fi

	# 4 check dtb is exist?
	if [ ! -e "$DTB_FILE" ]; then
		echo "$DTB_FILE isn't exist"
		exit 1
	fi
	echo "packet_check end"
}

build_tool()
{
	cd $WORKING_DIR/../../tools/coremsg
	if [ -e "coremsg" ]; then
		make clean
	fi
	make
	chmod 777 coremsg
}

unpacket_sdk()
{
	mkdir -p $SDK_UNPACKET_DIR

	#unpacket hi3093sdk.tar.gz
	tar xzf $SDK_PACKET_FILE -C $SDK_UNPACKET_DIR
	tar xzf $SECUREC_PACKER_FILE -C $SDK_UNPACKET_DIR
}

unpacket_cpio()
{
	#used to initrd.cpio.gz
	if [ -d "${CPIO_UNPACKER_DIR}" ];then
		sudo rm -rf $CPIO_UNPACKER_DIR
	fi
	mkdir -p $CPIO_UNPACKER_DIR
	cd $CPIO_UNPACKER_DIR
	zcat $CPIO_PACKER_FILE | cpio -dim
	cd $WORKING_DIR
	#used to ext4
	if [ -d "${TEMP_CPIO_UNPACKER_DIR}" ];then
		sudo rm -rf $TEMP_CPIO_UNPACKER_DIR
	fi
	mkdir -p $TEMP_CPIO_UNPACKER_DIR

	if [ -d "${FINAL_CPIO_DIR}" ];then
		sudo rm -rf $FINAL_CPIO_DIR
	fi
	cd $TEMP_CPIO_UNPACKER_DIR
	sudo zcat $CPIO_PACKER_FILE | cpio -dim
	cd $WORKING_DIR
}
generate_cms_bin()
{
	cd $EXT4CMS_DIR
	# check ext4cms is exist?
	if [ -e "$EXT4CMS_FILE" ]; then
		echo "$EXT4CMS_FILE is exist"
		rm Hi3093_ext4fs_cms.bin
		rm Hi3093_ext4fs.img
		rm Hi3093_ext4fs.img.g1.cms
		rm Hi3093_ext4fs.img.g2.cms
		rm crldata_g1.crl
	fi
	cp $EXT4_TARGET_BIN $EXT4CMS_DIR
	echo "Hi3093" >> Hi3093_ext4fs.img.g1.cms
	echo "Hi3093" >> Hi3093_ext4fs.img.g2.cms
	echo "Hi3093" >> crldata_g1.crl
	./generate_sign_image rootfs_cms.cfg
	dd if=Hi3093_ext4fs_cms.img of=Hi3093_ext4fs_cms.bin bs=1k count=36
	echo "generate_cms_bin end"
	cd $WORKING_DIR
}

update_cpio()
{
	echo "update_cpio begin"
	rm $CPIO_UNPACKER_DIR/init
	cp $WORKING_DIR/init $CPIO_UNPACKER_DIR
	mkdir -p $CPIO_UNPACKER_DIR/lib/net
	#cp ko
	cp $SDK_UNPACKET_DIR/securec_ko/ksecurec.ko $CPIO_UNPACKER_DIR/lib/net
	cp $SDK_UNPACKET_DIR/ko/log_drv.ko $CPIO_UNPACKER_DIR/lib/net
	cp $SDK_UNPACKET_DIR/ko/comm_drv.ko $CPIO_UNPACKER_DIR/lib/net
	cp $SDK_UNPACKET_DIR/ko/mdio_drv.ko $CPIO_UNPACKER_DIR/lib/net
	cp $SDK_UNPACKET_DIR/ko/msg_scm3_drv.ko $CPIO_UNPACKER_DIR/lib/net
	cp $SDK_UNPACKET_DIR/ko/usb-common.ko $CPIO_UNPACKER_DIR/lib/net
	cp $SDK_UNPACKET_DIR/ko/udc-core.ko $CPIO_UNPACKER_DIR/lib/net
	cp $SDK_UNPACKET_DIR/ko/configfs.ko $CPIO_UNPACKER_DIR/lib/net
	cp $SDK_UNPACKET_DIR/ko/libcomposite.ko $CPIO_UNPACKER_DIR/lib/net
	cp $SDK_UNPACKET_DIR/ko/usb_drv.ko $CPIO_UNPACKER_DIR/lib/net
	cp $SDK_UNPACKET_DIR/ko/dwc3.ko $CPIO_UNPACKER_DIR/lib/net
	cp $SDK_UNPACKET_DIR/ko/hw_lock_drv.ko $CPIO_UNPACKER_DIR/lib/net
	cp $SDK_UNPACKET_DIR/ko/mmc_core.ko $CPIO_UNPACKER_DIR/lib/net
	cp $SDK_UNPACKET_DIR/ko/mmc_block.ko $CPIO_UNPACKER_DIR/lib/net
	cp $SDK_UNPACKET_DIR/ko/emmc_drv.ko $CPIO_UNPACKER_DIR/lib/net
	cp $SDK_UNPACKET_DIR/ko/sdio_drv.ko $CPIO_UNPACKER_DIR/lib/net
	cp $SDK_UNPACKET_DIR/ko/localbus_drv.ko $CPIO_UNPACKER_DIR/lib/net
	cp $SDK_UNPACKET_DIR/ko/devmem_drv.ko $CPIO_UNPACKER_DIR/lib/net

	#cp tools
	cp $WORKING_DIR/../../tools/coremsg/coremsg $CPIO_UNPACKER_DIR/bin
	#mkdir newroot
	mkdir -p $CPIO_UNPACKER_DIR/mnt/newroot

	#rm unused file
	sudo rm -rf $CPIO_UNPACKER_DIR/usr/lib64/*
    
    libtirpc=$(find $TEMP_CPIO_UNPACKER_DIR/usr/lib64 -name 'libtirpc.so.*')
    cp $libtirpc $CPIO_UNPACKER_DIR/usr/lib64
    
    libpcre=$(find $TEMP_CPIO_UNPACKER_DIR/usr/lib64 -name 'libpcre.so.*' -o -name 'libpcre2-8.so.*')
    cp $libpcre $CPIO_UNPACKER_DIR/usr/lib64

    libreadline=$(find $TEMP_CPIO_UNPACKER_DIR/usr/lib64 -name 'libreadline.so.*')
    cp $libreadline $CPIO_UNPACKER_DIR/usr/lib64

    libglib=$(find $TEMP_CPIO_UNPACKER_DIR/usr/lib64 -name 'libglib-2.0.so.*')
    cp $libglib $CPIO_UNPACKER_DIR/usr/lib64

    cd $CPIO_UNPACKER_DIR/usr/lib64
    libpcre_base=$(basename $libpcre)
    if [[ $libpcre_base == libpcre.so.* ]]; then
        ln -s $libpcre_base libpcre.so.1
    elif [[ $libpcre_base == libpcre2-8.so.* ]]; then
        ln -s $libpcre_base libpcre2-8.so.0
    fi
    
    libreadline_base=$(basename $libreadline)
    ln -s $libreadline_base libreadline.so.8

    libglib_base=$(basename $libglib)
    ln -s $libglib_base libglib-2.0.so.0

    libtirpc_base=$(basename $libtirpc)
    ln -s $libtirpc_base libtirpc.so.3

	cd $CPIO_UNPACKER_DIR/usr
	sudo rm -rf bin/ games/ include/ lib/ libexec/ share/
	cd $CPIO_UNPACKER_DIR/lib
	sudo rm -rf depmod.d/ modprobe.d/ modules/
	cd $CPIO_UNPACKER_DIR
	find . | cpio -H newc -o | gzip -c > ../initrd_boot.cpio.gz
	cd $WORKING_DIR
	echo "update_cpio end"
}

update_cpio_for_ext4()
{
	echo "update_cpio_for_ext4 begin"
	#mkdir -p $TEMP_CPIO_UNPACKER_DIR/lib/net

    #create symbolic link
	pushd $TEMP_CPIO_UNPACKER_DIR/usr/lib64
	ln -s $libpcre_base libpcre2-8.so.0
	popd

	#cp ko
	cp $SDK_UNPACKET_DIR/ko/gmac_drv.ko $TEMP_CPIO_UNPACKER_DIR/lib/modules
	cp $SDK_UNPACKET_DIR/ko/peci_drv.ko $TEMP_CPIO_UNPACKER_DIR/lib/modules
	cp $SDK_UNPACKET_DIR/ko/pci_fix_drv.ko $TEMP_CPIO_UNPACKER_DIR/lib/modules
	cp $SDK_UNPACKET_DIR/ko/pcie_hisi02_drv.ko $TEMP_CPIO_UNPACKER_DIR/lib/modules

	#cp tools
	cp $WORKING_DIR/../../tools/coremsg/coremsg $TEMP_CPIO_UNPACKER_DIR/bin

	cp $WORKING_DIR/hi3093_init.sh $TEMP_CPIO_UNPACKER_DIR
	cp $WORKING_DIR/hi3093_upgrade.sh $TEMP_CPIO_UNPACKER_DIR
	cp $WORKING_DIR/initrd_boot.cpio.gz $TEMP_CPIO_UNPACKER_DIR/boot
	cp $WORKING_DIR/link_emmc_devs $TEMP_CPIO_UNPACKER_DIR
	cp $UIMAGE_FILE $TEMP_CPIO_UNPACKER_DIR/boot
	cp $DTB_FILE $TEMP_CPIO_UNPACKER_DIR/boot

	#cp openamp library
	if [ -d "${OPEN_AMP_OUTPUT_DIR}/usr/lib64" ]; then
	    cp -r $OPEN_AMP_OUTPUT_DIR/usr/lib64/* $TEMP_CPIO_UNPACKER_DIR/usr/lib64
	fi

	#cp custom file
	if [ -d "${CUSTOM_DIR}/usr" ]; then
		cp -r $CUSTOM_DIR/usr $TEMP_CPIO_UNPACKER_DIR
	fi
	if [ -e "$CUSTOM_DIR/hi3093_product.sh" ]; then
		cp $CUSTOM_DIR/hi3093_product.sh $TEMP_CPIO_UNPACKER_DIR
	fi

	cd $TEMP_CPIO_UNPACKER_DIR

	if [ "$SYS_TYPE" == "bm" ] ; then
		echo "cp Baremetal Special file"
	elif [ "$SYS_TYPE" == "oe_bm" ]; then
		echo "cp openEuler+Baremetal(2+2) Special file"
		if [ -e "$BAREMETAL_OUTPUT_DIR"/mcs_rpmsg/mcs_km.ko ]; then
			cp $BAREMETAL_OUTPUT_DIR/mcs_rpmsg/mcs_km.ko $TEMP_CPIO_UNPACKER_DIR/lib/modules
		else
			echo "mcs_km.ko isn't exit"
		fi
	elif [ "$SYS_TYPE" == "oe_libck" ]; then
		echo "cp openEuler+Libck(3+1) Special file"
		if [ -e "$LIBCK_OUTPUT_DIR"/mcs_rpmsg/mcs_km.ko ]; then
			cp $LIBCK_OUTPUT_DIR/mcs_rpmsg/mcs_km.ko $TEMP_CPIO_UNPACKER_DIR/lib/modules
		else
			echo "mcs_km.ko isn't exit"
		fi
	elif [ "$SYS_TYPE" == "oe_uniproton" ]; then
		echo "cp openEuler+Uniproton(3+1) Special file"
		if [ -e "$UNIPROTON_OUTPUT_DIR"/mcs_km.ko ]; then
			cp $UNIPROTON_OUTPUT_DIR/mcs_km.ko $TEMP_CPIO_UNPACKER_DIR/lib/modules
		else
			echo "mcs_km.ko isn't exit"
		fi
	else
		echo "cp openEuler Special file"
	fi

    files=$(grep "115200 ttyAMA0" -rli ./)
    if [ -n "$files" ]; then
        for file in $files; do
            sed -i "s/115200 ttyAMA0/115200 ttyS0 xterm/g" "$file"
        done
    else
        echo "No files found containing '115200 ttyAMA0'"
    fi

	files=$(grep "ttyAMA0:" -rli ./)
    if [ -n "$files" ]; then
        for file in $files; do
            sed -i "s/ttyAMA0:/ttyS0:/g" "$file"
        done
    else
        echo "No files found containing 'ttyAMA0:'"
    fi

	sed -i '$a\sh /hi3093_init.sh' etc/init.d/rc.sysinit
	cd $WORKING_DIR
	echo "update_cpio_for_ext4 end"
}

BLOCKSIZE=1024
EXT4_CNT=378k
EXT4_IMAGE_MOUNT_DIR=$WORKING_DIR/temp_mount 
make_ext4_image()
{
	dd if=/dev/zero of=$EXT4_TARGET_BIN bs=$BLOCKSIZE count=$EXT4_CNT
	mkfs.ext4 -b 4096 $EXT4_TARGET_BIN
	if [ -d "$EXT4_IMAGE_MOUNT_DIR" ];then
		rm -rf $EXT4_IMAGE_MOUNT_DIR
	fi

	#mount ext4
	sudo mkdir -p $EXT4_IMAGE_MOUNT_DIR
	ls -l
	sudo mount -t ext4 -o loop $EXT4_TARGET_BIN $EXT4_IMAGE_MOUNT_DIR
	sudo cp -r $TEMP_CPIO_UNPACKER_DIR/* $EXT4_IMAGE_MOUNT_DIR
	sudo umount $EXT4_IMAGE_MOUNT_DIR
}

image_pack()
{
	if [ -e "$WORKING_DIR"/output ]; then
		rm -rf $WORKING_DIR/output
	fi
	mkdir -p $WORKING_DIR/output
	cp $EXT4_TARGET_BIN $WORKING_DIR/output
	cp $EXT4CMS_FILE $WORKING_DIR/output

	cd $WORKING_DIR/output
	tar -czvf $TARGET_FILE * --format=gnu
	mv $TARGET_FILE $OUTPUTDIR
	cd $WORKING_DIR
}

delete_temp_file()
{
	rm Hi3093_ext4fs.img
	rm initrd_boot.cpio.gz
	sudo rm -rf cpio_dir/ hi3093sdk/ temp_mount/
	mv tmp_cpio_dir cpio_fs
}

#main process
build_tool
packet_check
unpacket_sdk
unpacket_cpio
update_cpio
update_cpio_for_ext4
make_ext4_image
generate_cms_bin
image_pack
delete_temp_file

exit 0
