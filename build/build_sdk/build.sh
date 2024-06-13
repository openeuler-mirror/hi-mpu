#!/bin/bash
# Copyright © Hisilicon Technologies. Co, Ltd. 2023-2023. All rights reserved.

set -e

SDK_BUILD_DIR=$(pwd)

source $SDK_BUILD_DIR/../version_${KERNEL_VERSION_MAIN}
source $SDK_BUILD_DIR/../build_exports

SDK_INSTALL_DIR=$SDK_BUILD_DIR/ko
# STRIP
STRIP_EXEC=$STRIP

export SRC_DIR=$SDK_BUILD_DIR/../../src
export SDK_ROOT=$SDK_BUILD_DIR/../../src/non_real_time/drivers

#Modules won't be compiled
EXCLUDE_MODULE_LIST="Makefile.cfg"
# modules dir name,by depend order,else cause compile warning and insmod fail
MODULE_ORDER_LIST='log comm board_cfg timer watchdog sys_info bmcrtc mdio \
                   gmac gpio hw_lock spi i2c i2c_hi309x ipmb bt kcs sol wdi p80 peci pwm sfccom \
                   uart_connect uart dboot m3_loader sdio adc usb_core usb ddrc dice djtag edma trng \
                   efuse norflash_1711 local_bus cpld local_bus_sys m3 pcie_fix pcie_host \
                   pcie_comm usb_device mctp msg_scm3 vce hisi_can virtual_cdev_veth dfx devmem'

function skip_exclude_module()
{
	for exclude_module in $EXCLUDE_MODULE_LIST
	do
		if [ "$1" == "$exclude_module" ]
		then
			echo skip $exclude_module compiling...
			return 1
		fi
	done

	return 0
}

#need abosolute path
SDK_MODULE_SYMVERS=${SDK_BUILD_DIR}/../../tempsymvers/sdk
if [ -d "${SDK_MODULE_SYMVERS}" ];then
	rm -rf $SDK_MODULE_SYMVERS
fi
mkdir -p $SDK_MODULE_SYMVERS
chown ${USER} $SDK_MODULE_SYMVERS

SECUREC_MODULE_SYMVERS=${SDK_BUILD_DIR}/../../tempsymvers/securec
KBUILD_EXTRA_SYMBOLS+="$SECUREC_MODULE_SYMVERS/Module.symvers "
KBUILD_EXTRA_SYMBOLS+="$SDK_MODULE_SYMVERS/Module.symvers"
export KBUILD_EXTRA_SYMBOLS


if [ -d "$SDK_INSTALL_DIR" ];then
	rm -f $SDK_INSTALL_DIR/*
else
	mkdir $SDK_INSTALL_DIR
fi

for module in $MODULE_ORDER_LIST
do
	echo $module
	skip_exclude_module $module

	if [ $? -eq 1 ]
	then
		echo "$module : skip compiling since it's in excluding module list!"
		continue
	fi

	if [ ! -e "$SDK_ROOT"/"$module"/Makefile ]
	then
		echo "$module: no Makefile, skip this module"
		continue
	fi

	echo "Building $module ..."
	echo "${COMPILE_PROCESS_NUM}"
	cp -r $SDK_ROOT/$module ${KERNEL_PATH}
	cd ${KERNEL_PATH}/$module && echo "CWD: $(pwd)" && make clean && make
	exit_if_error $? $module
	cp -r ${KERNEL_PATH}/$module/* $SDK_ROOT/$module
	cd $SDK_ROOT/$module
	if [ "${KERNEL_VERSION_MAIN}" == "4.4" ];then
		cat Module.symvers > $SDK_MODULE_SYMVERS/Module.symvers
	elif [ "${KERNEL_VERSION_MAIN}" == "5.10" ];then
		cat Module.symvers >> $SDK_MODULE_SYMVERS/Module.symvers
	fi

	find . -name '*.ko' -exec $STRIP_EXEC -d {} \;
	find . -name '*.ko' -exec cp {} $SDK_INSTALL_DIR \;
done
printf "\r\n\r\n\r\n################################### Finish to build driver module###################################\r\n"

#copy Module.symvers to build path
cp $SDK_MODULE_SYMVERS/Module.symvers $SDK_BUILD_DIR/

