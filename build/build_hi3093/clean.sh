#!/bin/bash
# Copyright © Hisilicon Technologies. Co, Ltd. 2023-2023. All rights reserved.

set -e

WORKING_DIR=$(pwd)
KERNEL_SRC_DIR=${WORKING_DIR}/open_source/linux5.10
SDK_BUILD_DIR=${WORKING_DIR}/build/build_sdk
OUT_PUT_DIR=${WORKING_DIR}/build/output

clean_kernel()
{
    echo "clean_kernel begin"

    if [ -e "$KERNEL_SRC_DIR"/arch/arm64/boot/Image ]; then
        rm $KERNEL_SRC_DIR/arch/arm64/boot/Image
    fi

    if [ -e "$KERNEL_SRC_DIR"/include/trace/trace.h ]; then
        rm $KERNEL_SRC_DIR/include/trace/trace.h
    fi

    cd $KERNEL_SRC_DIR
    make clean
    echo "clean_kernel end"
}

clean_sdk()
{
    echo "clean_sdk begin"
    cd $SDK_BUILD_DIR
	./clean_sdk.sh
	echo "clean_sdk end"
}

clean_output()
{
    echo "clean_output begin"
    if [ -d "$OUT_PUT_DIR" ]; then
        rm -rf $OUT_PUT_DIR
    fi
	echo "clean_output end"
}

clean_kernel
clean_sdk
clean_output