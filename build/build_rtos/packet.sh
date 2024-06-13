#!/bin/bash
# Copyright © Hisilicon Technologies. Co, Ltd. 2023-2023. All rights reserved.

PROJECT_PATH=$(pwd)/../..
OUTPUTDIR=$PROJECT_PATH/build/output

set -e

if [ ! -d "$OUTPUTDIR" ];then
	mkdir -p $OUTPUTDIR
fi

DTB=hi1711_asic.dtb
function hulk_packet()
{
	local kernel_file="kernel_image.tar.gz"
	tar -czvf ${kernel_file} uImage $DTB --format=gnu
	mv ${kernel_file} ${OUTPUTDIR}
}

printf "\r\n\r\n\r\n################################### Begin to packet####################################\r\n"
hulk_packet
printf "\r\n\r\n\r\n################################### Finish to packet###################################\r\n"
