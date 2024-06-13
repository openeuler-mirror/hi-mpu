#!/bin/bash
# Copyright © Hisilicon Technologies. Co, Ltd. 2023-2023. All rights reserved.

set -e

SDK_BUILD_DIR=$(pwd)

source ${SDK_BUILD_DIR}/../build_exports

SRCDIR=${SDK_BUILD_DIR}/../../platform
SECUREC_INSTALL_DIR=$SDK_BUILD_DIR/securec_ko
STRIP_EXEC=$STRIP

# copy securec Makefile adapter
if [ "${KERNEL_VERSION_MAIN}" == "5.10" ];then
	mv ${SRCDIR}/${SECUREC_PLATFORM}/src/Makefile ${SRCDIR}/${SECUREC_PLATFORM}/src/Makefile_old
	cp -r ${SDK_BUILD_DIR}/../../src/non_real_time/adapter_for_hi3093/securec_5.10_makefile ${SRCDIR}/${SECUREC_PLATFORM}/src/Makefile
fi

cd ${SRCDIR}/${SECUREC_PLATFORM}/src

# masking Makefile 110 line
sed -i '110c # ARCH:=$(shell getconf LONG_BIT)' Makefile
sed -i '177c KERNELDIR := ${KERNEL_PATH}/' Makefile
LINE_527_CONTENT=$(cat memcpy_s.c | sed -n '527p')
if [[ "${LINE_527_CONTENT}" != "MODULE_LICENSE (\"GPL\");" ]];then
	sed -i '527i MODULE_LICENSE ("GPL");' memcpy_s.c
fi

printf "\r\n\r\n\r\n################################### Begin to build linux kernel driver#############################\r\n"
if [ -d "$SECUREC_INSTALL_DIR" ];then
	rm -f $SECUREC_INSTALL_DIR/*
else
	mkdir $SECUREC_INSTALL_DIR
fi

#prepare Module.symvers safe check
SECUREC_MODULE_SYMVERS=${SDK_BUILD_DIR}/../../tempsymvers/securec

if [ -d "${SECUREC_MODULE_SYMVERS}" ];then
	rm -rf $SECUREC_MODULE_SYMVERS
fi
${SUDO}mkdir -p $SECUREC_MODULE_SYMVERS
${SUDO}chown ${USER} $SECUREC_MODULE_SYMVERS

echo "Building Securec ..."
echo "${COMPILE_PROCESS_NUM}"
cp -r $SRCDIR/${SECUREC_PLATFORM} ${KERNEL_PATH}
cd ${KERNEL_PATH}/${SECUREC_PLATFORM}/src && echo "CWD: $(pwd)" && make clean && make
exit_if_error $? ${SECUREC_PLATFORM}
cp -r ${KERNEL_PATH}/${SECUREC_PLATFORM}/src/* $SRCDIR/${SECUREC_PLATFORM}/src
cd $SRCDIR/${SECUREC_PLATFORM}/src

if [ "${KERNEL_VERSION_MAIN}" == "5.10" ];then
	cp Module.symvers $SECUREC_MODULE_SYMVERS/Module.symvers
fi

find . -name '*.ko' -exec $STRIP_EXEC -d {} \;
find . -name '*.ko' -exec cp {} $SECUREC_INSTALL_DIR \;

make clean
# restore securec Makefile
if [ "${KERNEL_VERSION_MAIN}" == "5.10" ];then
	rm -rf ${SRCDIR}/${SECUREC_PLATFORM}/src/Makefile
	mv ${SRCDIR}/${SECUREC_PLATFORM}/src/Makefile_old ${SRCDIR}/${SECUREC_PLATFORM}/src/Makefile
fi
