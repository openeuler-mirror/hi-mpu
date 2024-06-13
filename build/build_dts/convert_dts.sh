#!/bin/bash
# Copyright © Hisilicon Technologies. Co, Ltd. 2023-2023. All rights reserved

set -e

WORKING_DIR=$(pwd)
DTB_BUILD_DIR=${WORKING_DIR}/build/build_rtos
DTS_CONVERT_PATH=${WORKING_DIR}/../../tools/dts
SYS_TYPE=$1

dts_convert_h()
{
    cd ${DTS_CONVERT_PATH}
    
    if [ "${SYS_TYPE}" == "oe_bm" ]; then
        DTS_PATH=${WORKING_DIR}/../build_rtos/hi3093_mcs_2with2.dts
        python3 ./gen_defines.py --dts ${DTS_PATH} --dtc-flags DTC_FLAGS --bindings-dirs bindings --header-out device_resource.h  --isolate-text hi3093.txt
    fi

    if [ "${SYS_TYPE}" == "oe_uniproton" ]; then
        DTS_PATH=${WORKING_DIR}/../build_rtos/hi3093_mcs_3with1.dts
        python3 ./gen_defines.py --dts ${DTS_PATH} --dtc-flags DTC_FLAGS --bindings-dirs bindings --header-out device_resource.h  --isolate-text hi3093.txt
    fi

    if [ "${SYS_TYPE}" == "oe_libck" ]; then
        DTS_PATH=${WORKING_DIR}/../build_rtos/hi3093_mcs_3with1.dts
        python3 ./gen_defines.py --dts ${DTS_PATH} --dtc-flags DTC_FLAGS --bindings-dirs bindings --header-out device_resource.h  --isolate-text hi3093.txt
    fi

    if [ -e "${DTS_CONVERT_PATH}"/device_resource.h ]; then
        cp ${DTS_CONVERT_PATH}/device_resource.h ${WORKING_DIR}/../../src/real_time/drivers/include
        rm ${DTS_CONVERT_PATH}/device_resource.h
    else
        echo "covert file device_resource.h : failed"
    fi
}

dts_convert_h