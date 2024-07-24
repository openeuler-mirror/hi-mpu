# **hi-mpu**
# 介绍
	hi-mpu是mpu系列芯片的开源仓，该仓提供了芯片驱动、驱动依赖以及便以构建工程的源代码。


# **目录整体结构**
| 一层 | 二层 | 三层 | 描述 |
|--|--|--|--|
|build  |  |  |构建脚本目录	|
|custom  |  |  |用户定制文件存放目录	|
| | open_euler |  |	存放openEuler提供的三个基础组件：文件系统 *cpio.gz、内核镜像 zImage、内核配置文件 defconfig-kernel|
| | usr |  | 存放用户定制化文件目录	|
|open_source  |  |  ||
| | arm-trusted-firmware-2.7 |  |arm-trusted-firmware-2.7源码目录|
| | libmetal |  |混合部署libmetal源码目录|
| | mcs |  |混合部署mcs驱动源码目录|
| | musl |  |musl库源码目录|
| | open-amp |  |混合部署open-amp框架源码目录|
| | u-boot |  |u-boot源码目录|
|platform  |  |  |	平台软件目录|
| | osal |  |适配层代码目录|
| |  | uniproton |uniproton接口适配代码目录|
| | securec |  |安全c库目录|
|src  |  |  ||
| | non_real_time |  |非实时侧(openEuler)侧驱动相关代码目录|
| |  real_time |  |实时侧驱动(uniproton)代码相关目录|
| |  patches |  |开源代码相关patch|
| |  samples |  |sample用例目录|
| |  | non_real_time |非实时侧(openEuler)用例目录|
| |  | real_time |实时侧(uniproton)驱动用例|
| tools |  |  |工具源码目录|
|  |  coremsg|  |coremsg代码目录|
|  |  dts|  |dts转换工具代码目录|
|  |  emmc_divide|  |emmc分区工具代码目录|
| doc |  |  |编译、升级相关文档|


# 快速上手
	1. 参考openEuler相关文档，在docker里编译出相应版本的zImage、*cpio.gz，然后和defconfig-kernel,并放至mpu_solution/custom/open_euler目录
    2. 在mpu_solution目录执行build.sh脚本编译相应版本，支持的版本为：
    	oe
        oe_uniproton
    3. 编译后的烧片包在build/build_hi3093/output目录，烧片包文件为：
    	Hi3093_ext4fs.img
        u-boot_rsa_4096.bin
        Hi3093_ext4fs_cms.bin
	4. 用步骤3编译好的烧片包进行烧写升级, 升级方法为：
    	将步骤3编译好的文件和l0fw_rsa_4096.bin和l1fw_rsa_4096.bin这两个文件通过ssh上传至单板/tmp目录，然后在/tmp目录执行 sh /hi3093_upgrade.sh 4096命令升级


# 参与贡献
	Fork 本仓库
	新建 Feat_xxx 分支
	提交代码
	新建 Pull Request

    
#  开源许可
 	src/non_real_time/drivers目录下的驱动相关代码使用 GPL v2.0 license
    src/real_time/drivers目录下的实时侧驱动代码使用 MulanPSL-2.0 license
