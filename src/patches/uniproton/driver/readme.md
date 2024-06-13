[TOC]

# 支持驱动使用说明

本文档用于解释说明 如何在社区Unipronton代码上添加3093的驱动代码。

## 一、配置外网代理(可选，外网用户请忽略)

内网用户需要配置代理访问gitee，外网用户可忽略：
export http_proxy="http://user:password@proxyhk.huawei.com:8080"
export https_proxy="http://user:password@proxyhk.huawei.com:8080"

## 二、安装依赖环境

下载编译器：[gcc-arm-10.3-2021.07-x86_64-aarch64-none-elf.tar.xz (gitee.com)](https://gitee.com/link?target=https%3A%2F%2Fdeveloper.arm.com%2F-%2Fmedia%2FFiles%2Fdownloads%2Fgnu-a%2F10.3-2021.07%2Fbinrel%2Fgcc-arm-10.3-2021.07-x86_64-aarch64-none-elf.tar.xz)

解压编译器：tar -xvf gcc-arm-10.3-2021.07-x86_64-aarch64-none-elf.tar.xz -C /opt/buildtools

## 三、下载Uniproton代码

下载方式：wget https://gitee.com/openeuler/UniProton/repository/archive/v1.0.1.zip --no-check-certificate

解压：unzip v1.0.1.zip， 解压后目录名为UniProton-v1.0.1

## 四、打patch

将driver-support-uniproton.patch拷贝到解压的UniProton-v1.0.1根目录下。

执行命令 patch -p1 < driver-support-uniproton.patch

## 五、编译

cd UniProton-v1.0.1/demos/hi3093/build

执行 sh build_app.sh，生成hi3093.bin

```
非root用户： 使用非root账号编译时，会出现cannot find -lssp_nonshared 和 -lssp. 建议用root权限的账号执行。
```

