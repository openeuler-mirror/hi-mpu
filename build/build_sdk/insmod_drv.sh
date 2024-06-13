#!/bin/bash
# Copyright © Hisilicon Technologies. Co, Ltd. 2023-2023. All rights reserved.

if [ -n "$1" ]
then
	kopath=$1
else
	kopath="/ko"
fi


if [ -n "$2" ]
then
	binpath=$2
else
	binpath="/usr/bin"
fi

if [ -n "$3" ]
then
	scriptpath=$3
else
	scriptpath="/opt/pme/init"
fi

modprobe ipv6.ko
modprobe 8021q
modprobe fat
modprobe vfat
modprobe scsi_mod
modprobe sd_mod
modprobe usb-common
modprobe usbcore
modprobe usb-storage
modprobe xhci-hcd
############### base driver start##################
cd $kopath

insmod gpio_drv.ko
insmod mdio_drv.ko
insmod gmac_drv.ko
insmod sys_info_drv.ko
############### base driver end##################

###############  ext driver start##################
insmod i2c_drv.ko
insmod ipmb_drv.ko

insmod p80_drv.ko
insmod bt_drv.ko
insmod sol_drv.ko

insmod dboot_drv.ko
insmod pwm_drv.ko
insmod adc_drv.ko
insmod bmcrtc_drv.ko

insmod uart_core.ko
insmod uartconnect_drv.ko
insmod uart_drv.ko
insmod peci_drv.ko
insmod wdi_drv.ko
insmod localbus_drv.ko
insmod spi_drv.ko 
insmod cpld_drv.ko 

insmod djtag_drv.ko
insmod trng_drv.ko
insmod efuse_drv.ko

insmod m3_loader_drv.ko
insmod pci_fix_drv.ko
insmod mctp_drv.ko
insmod msg_scm3_drv.ko
##########insmod usb device drvice start#########
insmod udc-core.ko
insmod configfs.ko
insmod libcomposite.ko
insmod usb_drv.ko
insmod dwc3.ko
insmod vce_drv.ko
insmod virtual_usb_device.ko
##########insmod usb device drvice end#########
insmod dfx_drv.ko

##########modify ipfrag_time #########
echo 5 > /proc/sys/net/ipv4/ipfrag_time

###############  ext driver end##################
cd $scriptpath
./link_emmc_devs

cd /usr/bin
./uart_flowoff&
