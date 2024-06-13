#!/bin/bash
# Copyright © Hisilicon Technologies. Co, Ltd. 2023-2023. All rights reserved.

set -e

SDK_BUILD_DIR=$(pwd)
TEMP_SYMVERS_DIR=$SDK_BUILD_DIR/../../tempsymvers
MODULE_ORDER_LIST='log comm board_cfg timer watchdog sys_info bmcrtc mdio \
                   gmac gpio hw_lock spi i2c i2c_hi309x ipmb bt kcs sol wdi p80 peci pwm sfccom \
                   uart_connect uart dboot m3_loader sdio adc usb_core usb ddrc dice djtag edma trng \
                   efuse norflash_1711 local_bus cpld local_bus_sys m3 pcie_fix pcie_host \
                   pcie_comm usb_device mctp msg_scm3 vce hisi_can virtual_cdev_veth dfx driver_test'
export SDK_ROOT=$SDK_BUILD_DIR/../../src/non_real_time/drivers

for module in $MODULE_ORDER_LIST
do
    if [[ "$module" == "\\" ]]; then
        continue
    fi
    echo "Clean: $module"
    if [ -d "$SDK_ROOT"/"$module" ]; then
		cd $SDK_ROOT/$module
        find ./ -name '*.mod.c' | xargs rm -rf
        find ./ -name '*.ko' | xargs rm -rf
        find ./ -name '*.mod' | xargs rm -rf
        find ./ -name '*.o' | xargs rm -rf
        find ./ -name '*.order' | xargs rm -rf
        find ./ -name '*.symvers' | xargs rm -rf
    fi
done
if [ -d "$TEMP_SYMVERS_DIR" ]; then
    rm -rf $TEMP_SYMVERS_DIR
fi
