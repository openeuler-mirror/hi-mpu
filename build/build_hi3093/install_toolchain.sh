#!/bin/bash
# Copyright © Hisilicon Technologies. Co, Ltd. 2023-2023. All rights reserved
set -e

HI3093_DIR=/home/${USER}/hi3093_tool
TOOLCHAIN_DIR=${HI3093_DIR}/toolchain

install_toolchain()
{
    if [ ! -d "$TOOLCHAIN_DIR" ]; then
        mkdir -p $TOOLCHAIN_DIR
    fi

    echo "install_toolchain begin"
    # 1 check $TOOLCHAIN_DIR/sysroots is exist
    if [ -d "$TOOLCHAIN_DIR"/sysroots ]; then
        echo "$TOOLCHAIN_DIR/sysroots is exist"
    fi

    # 2 check -toolchain-22.03.sh is exist?
    pushd "${TOOLCHAIN_DIR}"
    if [ ! -e "$TOOLCHAIN_DIR"/openeuler-glibc-x86_64-openeuler-image-aarch64-qemu-aarch64-toolchain-22.03-LTS-SP3.sh ]; then
        wget --no-check-certificate https://mirror.truenetwork.ru/openeuler/openEuler-22.03-LTS-SP3/embedded_img/aarch64/qemu-aarch64/openeuler-glibc-x86_64-openeuler-image-aarch64-qemu-aarch64-toolchain-22.03-LTS-SP3.sh
        sh openeuler-glibc-x86_64-openeuler-image-aarch64-qemu-aarch64-toolchain-22.03-LTS-SP3.sh -d ${TOOLCHAIN_DIR} -y
    fi
    popd

    source ${TOOLCHAIN_DIR}/environment-setup-aarch64-openeuler-linux

    echo "install_toolchain end"
}

install_toolchain
