#!/bin/bash
# Copyright © Hisilicon Technologies. Co, Ltd. 2023-2023. All rights reserved
set -e

BUILD_DIR=$(pwd)
LIBPCAP_SRC_DIR=${BUILD_DIR}/libpcap
TCPDUMP_SRC_DIR=${BUILD_DIR}/tcpdump
OUTPUTDIR_BUILD=${BUILD_DIR}/output
BUILD_HI3093_DIR=../build_hi3093

install_toolchain()
{
	pushd "$BUILD_HI3093_DIR"
	source install_toolchain.sh
	popd
}

download_libpcap()
{
	echo "download_libpcap begin"
	cd ${BUILD_DIR}
	if [ -d "$LIBPCAP_SRC_DIR" ]; then
		echo "$LIBPCAP_SRC_DIR is exist"
		return 0
	fi

	wget --no-check-certificate https://gitee.com/src-openeuler/libpcap/repository/archive/openEuler-22.03-LTS-SP3.zip
	unzip openEuler-22.03-LTS-SP3.zip
	rm openEuler-22.03-LTS-SP3.zip
	mv libpcap-openEuler-22.03-LTS-SP3 libpcap
	cd libpcap
	tar -xvf libpcap-1.10.3.tar.gz
}

download_tcpdump()
{
	cd ${BUILD_DIR}
	echo "download_tcpdump begin"
	# 1 check uboot is exist?
	if [ -d "$TCPDUMP_SRC_DIR" ]; then
		echo "TCPDUMP_SRC_DIR is exist"
		return 0
	fi

	wget --no-check-certificate https://gitee.com/src-openeuler/tcpdump/repository/archive/openEuler-22.03-LTS-SP3.zip
	unzip openEuler-22.03-LTS-SP3.zip
	rm openEuler-22.03-LTS-SP3.zip
	mv tcpdump-openEuler-22.03-LTS-SP3 tcpdump
	cd tcpdump
	tar -xvf tcpdump-4.99.3.tar.gz
	echo "download_tcpdump end"
}

compile_libpcap()
{
	echo "compile_libpcap begin"
	cd ${LIBPCAP_SRC_DIR}/libpcap-1.10.3
	git apply ../backport-0003-pcap-linux-apparently-ctc-interfaces-on-s390-has-eth.patch
	git apply ../backport-pcap-config-mitigate-multilib-conflict.patch

	if [ -d "build" ]; then
		rm -rf build
	fi

	mkdir build
	cd build
	../configure --prefix=$(pwd)/install --target=aarch64-openeuler-linux --host=aarch64-openeuler-linux --build=x86_64-linux --with-libtool-sysroot=$SDKTARGETSYSROOT
	make && make install
	echo "compile_libpcap end"
}

compile_tcpdump()
{
	echo "compile_tcpdump begin"
	cd ${TCPDUMP_SRC_DIR}/tcpdump-4.99.3
	git apply ../backport-0002-Use-getnameinfo-instead-of-gethostbyaddr.patch
	git apply ../backport-0003-Drop-root-priviledges-before-opening-first-savefile-.patch
	git apply ../backport-0007-Introduce-nn-option.patch
	git apply ../backport-0009-Change-n-flag-to-nn-in-TESTonce.patch
	git apply ../backport-CVE-2023-1801.patch
	git apply ../tcpdump-Add-sw64-architecture.patch

	if [ -d "build" ]; then
		rm -rf build
	fi

	mkdir build
	cd build
	cp -rf ${LIBPCAP_SRC_DIR}/libpcap-1.10.3/build/install libpcap
	../configure  --target=aarch64-openeuler-linux --host=aarch64-openeuler-linux --build=x86_64-linux --with-libtool-sysroot=$SDKTARGETSYSROOT CFLAGS="-I./libpcap/include/ -L./libpcap/lib/"
	make

	if [ ! -d "$OUTPUTDIR_BUILD" ]; then
		mkdir -p ${OUTPUTDIR_BUILD}
	fi

	cp ${TCPDUMP_SRC_DIR}/tcpdump-4.99.3/build/tcpdump ${OUTPUTDIR_BUILD}/

	echo "compile_tcpdump end"
}

clean_dir()
{
    echo "clean_dir"
	rm -rf ${LIBPCAP_SRC_DIR}
	rm -rf ${TCPDUMP_SRC_DIR}
	echo "clean_dir"
}

install_toolchain
download_libpcap
download_tcpdump
compile_libpcap
compile_tcpdump
clean_dir