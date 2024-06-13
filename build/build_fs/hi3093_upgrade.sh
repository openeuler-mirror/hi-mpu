#!/bin/bash
# Copyright © Hisilicon Technologies. Co, Ltd. 2023-2023. All rights reserved.

set -e

BLK_SIZE=1k
ROOTFS_OFF=0
ROOTFS_HDR_OFF=389120
L0FW_OFF=390144
L1FW_OFF=390400
UBOOT_OFF=390912

if [ "$1" == '2048' ];then
	WIDTH=2048
elif [ "$1" == '4096' ];then
	WIDTH=4096
else
	echo 'width param error. use "2048" or "4096"'
	exit -1
fi

if [[ "$2" == "uboot_cms" ]]
then
	UBOOT_FILE=u-boot_cms.bin
else
	UBOOT_FILE=u-boot_rsa_${WIDTH}.bin
fi

function update_temp_gpp()
{
	if [ -f "l0fw_rsa_${WIDTH}.bin" ]
	then
		echo 'update temp L0FW...'
		dd if=l0fw_rsa_${WIDTH}.bin of=/dev/nvm_temp bs=$BLK_SIZE count=256 seek=$L0FW_OFF
	else
		echo "l0fw_rsa_${WIDTH}.bin not found. no update."
	fi

	if [ -f "l1fw_rsa_${WIDTH}.bin" ]
	then
		echo 'update temp L1FW...'
		dd if=l1fw_rsa_${WIDTH}.bin of=/dev/nvm_temp bs=$BLK_SIZE count=512 seek=$L1FW_OFF
	else
		echo "l1fw_rsa_${WIDTH}.bin not found. no update."
	fi
	
	if [ -f "$UBOOT_FILE" ]
	then
		echo 'update temp u-boot...'
		dd if=$UBOOT_FILE of=/dev/nvm_temp bs=$BLK_SIZE count=512 seek=$UBOOT_OFF
	else
		echo "$UBOOT_FILE not found. no update."
	fi
	
	if [ -f "Hi3093_ext4fs_cms.bin" ]
	then
		echo 'update temp rootfs_hdr...'
		dd if=Hi3093_ext4fs_cms.bin of=/dev/nvm_temp bs=$BLK_SIZE count=36 seek=$ROOTFS_HDR_OFF
	else
		echo "Hi3093_ext4fs_cms.bin not found. no update."
	fi
	
	if [ -f "Hi3093_ext4fs.img" ]
	then
		echo 'update temp rootfs...'
		dd if=Hi3093_ext4fs.img of=/dev/nvm_temp bs=1M
	else
		echo "Hi3093_ext4fs.img not found. no update."
	fi
}

#get temp_gpp
sh /link_emmc_devs

#write data to temp_gpp
update_temp_gpp

#set update flag
coremsg setUpdFlag

#reboot
echo "emmc update success, please reboot your board..."

