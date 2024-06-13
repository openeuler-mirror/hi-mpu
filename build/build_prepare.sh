#!/bin/bash
# Copyright © Hisilicon Technologies. Co, Ltd. 2023-2023. All rights reserved.

export KERNEL_VERSION_MAIN=5.10

PROJECT_BUILD_DIR=$(pwd)
WORKE_DIR=${PROJECT_BUILD_DIR}/../

export KERNEL_SRC_DIR=${PROJECT_BUILD_DIR}/../open_source/linux5.10

function prepare_build()
{
	# copy securec dependency to kernel
	cp ${PROJECT_BUILD_DIR}/../platform/securec/include/* ${KERNEL_SRC_DIR}/include/linux/

	# copy kbox adapter to kernel
	cp -r ${PROJECT_BUILD_DIR}/../src/non_real_time/adapter_for_hi3093/include/kbox ${KERNEL_SRC_DIR}/include/linux/
}

export KERNEL_VERSION=5.10.0
if [ -f "${PROJECT_BUILD_DIR}"/release/release.sh ]; then
    cp -f ${PROJECT_BUILD_DIR}/release/release.sh ${WORKE_DIR}
fi

prepare_build
