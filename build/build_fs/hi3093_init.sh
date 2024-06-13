#!/bin/bash
# Copyright © Hisilicon Technologies. Co, Ltd. 2023-2023. All rights reserved.

set -e

echo "exec hi3093_init.sh"
insmod /lib/modules/gmac_drv.ko
ifconfig eth2 192.168.0.11 up

if [ -e "hi3093_product.sh" ]; then
	echo "exec hi3093_product.sh"
	./hi3093_product.sh
fi
