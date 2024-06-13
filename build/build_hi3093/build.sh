#!/bin/bash
# Copyright © Hisilicon Technologies. Co, Ltd. 2023-2023. All rights reserved
#packet hi3093

set -e

WORKING_DIR=$(pwd)
HI3093_TOOL_DIR=/home/${USER}/hi3093_tool
UBOOT_2020_DIR=${HI3093_TOOL_DIR}/u-boot-2020.01
TOOLCHAIN_DIR=${HI3093_TOOL_DIR}/toolchain
UBOOT_2020_DIR_ZIP=${HI3093_TOOL_DIR}/v2020.01.zip
DOCKER_FILEDIR=${WORKING_DIR}/custom/open_euler

DTB_BUILD_DIR=${WORKING_DIR}/build/build_rtos
DTB_FILE_NAME=${WORKING_DIR}/build/build_rtos/hi1711_asic.dtb
BUILD_HI3093_DIR=${WORKING_DIR}/build/build_hi3093
OPENSOURCE_DIR=${WORKING_DIR}/open_source
BUILD_OUTPUT_DIR=${WORKING_DIR}/build/output
BUILD_HI3093_OUTPUT=${WORKING_DIR}/build/build_hi3093/output
BUILD_BAREMETAL_DIR=${WORKING_DIR}/build/build_baremetal
BUILD_UNIPROTON_DIR=${WORKING_DIR}/build/build_uniproton
BUILD_LIBCK_DIR=${WORKING_DIR}/build/build_libck
BUILD_DTS_CONVERT_DIR=${WORKING_DIR}/build/build_dts

if read -t 15 -p $'please enter your System:
    oe:openEuler System
    bm:Baremetal System
    oe_bm:openEuler+Baremetal(2+2)System
    oe_libck:openEuler+Libck(3+1)System
    oe_uniproton:openEuler+Uniproton(3+1)System
    default:openEuler System\x0a:' SYS_TYPE
then
	echo "build begin"
else
	echo $'\x0asorry,too slow'
	exit 0
fi

prepare_check()
{
	echo "prepare_check begin"
	# 1 check cpio is exist?
	if [ ! -d "$DOCKER_FILEDIR" ]; then
		echo "$DOCKER_FILEDIR isn't exist"
		exit 1
	fi
	if [ ! -e "$DOCKER_FILEDIR"/openeuler-image-qemu-aarch64-*.rootfs.cpio.gz ]; then
		echo "$DOCKER_FILEDIR/openeuler-image-qemu-aarch64-*.rootfs.cpio.gz isn't exist"
		exit 1
	fi

	# 2 check cpio is zImage-5.10.0?
	if [ ! -e "$DOCKER_FILEDIR"/zImage-5.10.0 ]; then
		echo "DOCKER_FILEDIR/zImage-5.10.0 isn't exist"
		exit 1
	fi

	# 3 check cpio is zImage-5.10.0?
	if [ ! -e "$DOCKER_FILEDIR"/defconfig-kernel ]; then
		echo "DOCKER_FILEDIR/defconfig-kernel isn't exist"
		exit 1
	fi
	cp $DOCKER_FILEDIR/openeuler-image-qemu-aarch64-*.rootfs.cpio.gz $DOCKER_FILEDIR/initrd_boot.cpio.gz
	echo "prepare_check end"
}

prepare_dir()
{
	echo "prepare_dir begin"
	if [ ! -d "$HI3093_TOOL_DIR" ]; then
		mkdir -p $HI3093_TOOL_DIR
	else
		echo "$HI3093_TOOL_DIR is exist"
	fi

	if [ ! -d "$TOOLCHAIN_DIR" ]; then
		mkdir -p $TOOLCHAIN_DIR
	else
		echo "$TOOLCHAIN_DIR is exist"
	fi
	echo "prepare_dir end"
}

download_linux()
{
	echo "downloap_linux begin"
	# 1 check $OPENSOURCE_DIR/linux5.10 is exist
	if [ -d "$OPENSOURCE_DIR"/linux5.10 ]; then
		echo "$OPENSOURCE_DIR/linux5.10 is exist"
		return 0
	fi

	# 2 check -toolchain-22.03.sh is exist?
	pushd "${OPENSOURCE_DIR}"
	if [ ! -e "$OPENSOURCE_DIR"/5.10.0-177.0.0.zip ]; then
		wget --no-check-certificate https://gitee.com/openeuler/kernel/repository/archive/5.10.0-177.0.0.zip
	fi

	if [ ! -d "$OPENSOURCE_DIR"/5.10.0-177.0.0.zip ]; then
		unzip 5.10.0-177.0.0.zip
	fi
	mv kernel-5.10.0-177.0.0/ linux5.10
	popd

	echo "downloap_linux end"
}

make_linux()
{
	echo "make_linux begin"

	if [ ! -e "$OPENSOURCE_DIR"/linux5.10/arch/arm64/boot/Image ]; then
		pushd "${OPENSOURCE_DIR}/linux5.10"
		cp $DOCKER_FILEDIR/defconfig-kernel $OPENSOURCE_DIR/linux5.10/arch/arm64/configs/defconfig
		set +e
		patch -p1 -N < ../../src/patches/openEuler/kernel-22.03-lts-sp3-mmc.patch
		set -e
		make ARCH=arm64 defconfig
		make ARCH=arm64 CROSS_COMPILE=aarch64-openeuler-linux- -j32
		popd
	fi
	echo "make_linux end"
}

make_sdk()
{
	echo "make_sdk begin"
	if [ ! -e "$BUILD_OUTPUT_DIR"/hi3093sdk.tar.gz ]; then
		pushd "${WORKING_DIR}/build/build_sdk"
		./build_sdk.sh
		popd
	fi
	echo "make_sdk end"
}

make_uimage()
{
	echo "make_uimage begin"
	pushd "${DOCKER_FILEDIR}"
	./mkimage -A arm -O linux -T kernel -C none -a 0x98280000 -e 0x98280040 -n linux-5.10.0 -d zImage-5.10.0 uImage
	popd

	if [ ! -e "$DOCKER_FILEDIR"/uImage ]; then
		echo "make_uimage failed"
		exit 1
	fi

	if [ ! -d "${BUILD_OUTPUT_DIR}" ]; then
		echo "plase check sdk build is success?"
		exit 1
	fi

	mv ${DOCKER_FILEDIR}/uImage ${BUILD_OUTPUT_DIR}
	mv ${DOCKER_FILEDIR}/initrd_boot.cpio.gz ${BUILD_OUTPUT_DIR}
	echo "make_uimage end"
}

make_dtb()
{
	if [ -e "$DTB_FILE_NAME" ]; then
		rm $DTB_FILE_NAME
	fi

	pushd "$DTB_BUILD_DIR"
	if [ "$SYS_TYPE" == "oe_bm" ] ; then
		echo $'make "2with2" dtb begin'
		dtc -I dts -O dtb -o $DTB_FILE_NAME hi3093_mcs_2with2.dts
		echo $'make "2with2" dtb end'
	elif [ "$SYS_TYPE" == "oe_libck" -o $SYS_TYPE == "oe_uniproton" ]; then
		echo $'make "3with1" dtb begin'
		dtc -I dts -O dtb -o $DTB_FILE_NAME hi3093_mcs_3with1.dts
		echo $'make "3with1" dtb end'
	else
		echo "make openEuler or Baremetal dtb begin"
		dtc -I dts -O dtb -o $DTB_FILE_NAME hi3093.dts
		echo "make openEuler or Baremetal dtb end"
	fi
	popd
}

make_barematel_2with2()
{
	if [ -e "${WORKING_DIR}"/platform/securec/src/Makefile ]; then
		pushd "${WORKING_DIR}/platform/securec/src"
		make clean
		popd
	fi

	pushd "$BUILD_BAREMETAL_DIR"
	./build_application.sh oe_bm
	popd
}

make_uniproton_3with1()
{
	pushd "$BUILD_UNIPROTON_DIR"
	./build_uniproton.sh
	popd
}

make_libck_3with1()
{
	pushd "$BUILD_LIBCK_DIR"
	./build_application.sh
	popd
}

make_packet()
{
	if [ -d "${BUILD_HI3093_OUTPUT}" ]; then
		rm -rf $BUILD_HI3093_OUTPUT
	fi

	mkdir -p $BUILD_HI3093_OUTPUT
	cp $WORKING_DIR/build/output/u-boot_rsa_4096.bin $BUILD_HI3093_OUTPUT
	cp $WORKING_DIR/build/build_fs/output/* $BUILD_HI3093_OUTPUT
}

make_dts_tree()
{
	if [ ! -d "${BUILD_DTS_CONVERT_DIR}" ]; then
		echo "dts isn't exist"
		exit 1
	fi

	cd ${BUILD_DTS_CONVERT_DIR}

	./convert_dts.sh ${SYS_TYPE}
}

#make Baremetal System
if [ "$SYS_TYPE" == "bm" ]; then
	pushd $WORKING_DIR/build
	set +e
	source build_prepare.sh
	set -e
	popd

	echo "make Baremetal System begin"
    pushd "$BUILD_HI3093_DIR"
    source install_toolchain.sh
    popd

    pushd "$BUILD_HI3093_DIR"
    ./make_uboot.sh bm
    popd

    pushd "$BUILD_BAREMETAL_DIR"
    ./build_application.sh bm
    popd
    echo "make Baremetal System ^-^"
    exit 0
fi

#main process
prepare_check
prepare_dir

pushd "$BUILD_HI3093_DIR"
source install_toolchain.sh
popd

download_linux

##build_prepare
chmod u+x -R build
pushd "${WORKING_DIR}/build"
source build_prepare.sh
source build_exports
popd

make_linux
make_sdk
# make_uboot
pushd "$BUILD_HI3093_DIR"
./build_openamp.sh
./make_uboot.sh $SYS_TYPE
popd

make_uimage
make_dtb
make_dts_tree
if [ "$SYS_TYPE" == "oe_bm" ]; then
	make_barematel_2with2
fi

if [ "$SYS_TYPE" == "oe_uniproton" ]; then
	make_uniproton_3with1
fi

if [ "$SYS_TYPE" == "oe_libck" ]; then
	make_libck_3with1
fi

echo "entry >>>> ${WORKING_DIR}/build/build_fs"
pushd "${WORKING_DIR}/build/build_fs"
./build_hi3093_ext4.sh $SYS_TYPE
popd
echo "exit <<<< ${WORKING_DIR}/build/build_fs"

make_packet

echo "OpenEuler build seccuss ^-^"
exit 0
