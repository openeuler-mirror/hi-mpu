# 1. EMMC镜像构建

## 1.1 概述

eMMC镜像构建描述了生成eMMC镜像升级所需要的Hi3093_ext4fs.img、Hi3093_ext4fs_cms.bin和u-boot_rsa_4096.bin的方法。

由于openEuler Embedded构建过程需要基于openEuler操作系统，且需要安装较多系统工具和构建工具，为方便开发人员快速搭建构建环境，我们将构建过程所依赖的操作系统和工具封装到一个容器中，使得开发人员可以快速搭建一个构建环境，进而投入到代码开发中去，避免在准备环境阶段消耗大量时间。

**前提条件：**

1. 编译环境的准备请参考文档：[https://embedded.pages.openeuler.org/openEuler-22.03-LTS-SP3/yocto/oebuild.html](https://embedded.pages.openeuler.org/openEuler-22.03-LTS-SP3/yocto/oebuild.html)。

2. 下载依赖代码：
	- zephyr 的构建包含核心部分和外部 zephyr modules 部分，由于全部代码较大，需要从该链接中 [src-openEuler/zephyr](https://gitee.com/src-openeuler/zephyr)所提到的百度网盘路径下载zephyr_project_v3.2.0.tar.gz。
    - python3-pykwalify 在 openeuler 社区尚无相应的源码包，需要从上游下载[Download pykwalify-1.8.0.tar.gz](https://pypi.org/project/pykwalify/1.8.0/#files)。

**编译步骤如下**

**步骤1** ： 依次执行如下命令，初始化oebuild工作目录以及构建分支代码：
```
oebuild init sp3-buildwork -b openEuler-22.03-LTS-SP3
cd /home/user/sp3-buildwork
oebuild update
```

<span style="color:#0066cc;">**须知**</span>

1. 假设执行路径位于/home/user/，执行后根据提示进入对应目录：执行路径可以根据自己需求修改执行路径。
2. sp3-buildwork：存放目录。
3. openEuler-22.03-LTS-SP3为当前支持本构建的源码分支。
4. 执行oebuild update完成后，将在 /home/user/sp3-buildwork/src/ 目录下载好主构建源码，并初始化构建虚拟环境。

**步骤2：** 进入/home/user/sp3-buildwork/src目录，依次执行如下命令：
```
cd /home/user/sp3-buildwork/src
mkdir zephyrproject
mkdir python3-pykwalify
```

**步骤3：** 把依赖代码压缩包放在对应目录下：

- 把zephyr_project_v3.2.0.tar.gz放在对应oebuild工作目录的/home/user/sp3-buildwork/src/zephyrproject下。
- 把pykwalify-1.8.0.tar.gz放在对应oebuild工作目录的/home/user/sp3-buildwork/src/python3-pykwalify下。

**步骤4：** 进入oebuild工作目录，执行如下命令进行创建对应的编译配置文件：
```
oebuild generate -p qemu-aarch64 -f openeuler-mcs -d build_hi3093
```

**步骤5：** 进入构建容器内，执行如下命令构建镜像：
```
cd /home/user/sp3-buildwork/build/build_hi3093
oebuild bitbake
bitbake openeuler-image
```

输出件：

构建完成后，输出件见/home/user/sp3-buildwork/build/build_hi3093/output/[时间戳]，内容如下：
- openeuler-image-qemu-aarch64-xxx.rootfs.cpio.gz
- zImage-5.10.0

## 1.2 制作EMMC镜像
**步骤1** 下载该库代码至ubuntu。

**步骤2** 将概述中docker容器里面路径`/usr1/openeuler/src/yocto-meta-openeuler/meta-openeuler/recipes-kernel/linux/files/config/arm64`目录下的defconfig-kernel拷贝到`hi-mpu/custom/open_euler`目录下。

**步骤3** 将概述中输出件`openeuler-image-qemu-aarch64-xxx.rootfs.cpio.gz、zImage-5.10.0拷贝到hi-mpu/custom/open_euler`目录下。

**步骤4** 执行如下编译脚本：
```
cd hi-mpu
./build.sh
```
出现编译选项提示后，输入oe或直接按回车（默认编译选项是openEuler）。

**步骤5** 编译生成的文件在hi-mpu/build/build_hi3093/output目录下：
- u-boot_rsa_4096.bin: uboot。
- Hi3093_ext4fs_cms.bin: 文件系统签名。
- Hi3093_ext4fs.img: 文件系统，里面包含uImage和设备树。

**<span style="color:#0066cc;">须知</span>**

-	本解决方案不包括openEuler系统，为提升用户构建效率，一键构建脚本中会通过网络下载openEuler-22.03-LTS-SP3版本的内核源代码和工具链，用户可通过修改build.sh指定openEuler版本，openEuler系统部分的网络安全问题由用户负责。
- 需要使用者具有sudo权限的账号去执行build.sh脚本。
- 执行build.sh过程中可能会让输入账户密码。
- 脚本会创建~/hi3093_tool目录，里面存放编译所依赖的工具和编译链。
- `Hi3093_ext4fs_cms.bin和Hi3093_ext4fs.img在build/output/hi3093_ext4.tar.gz`压缩包中，也在`build/build_fs/output`目录下。
- 如果有额外需要打包到二级文件系统的ko、bin、so、脚本等文件，需要将文件放到`mpu_solution/custom/usr`对应目录，然后再执行编译脚本。
- 如果有需要在开机启动时执行的脚本，将对应脚本文件放到`mpu_solution/custom`目录下，并命名为hi3093_product.sh，然后再执行编译脚本，系统启动时将会自动调用此脚本。
- `mpu_solution/build/build_fs/cpio_fs`保存了文件系统压缩前的文件及目录结构。

## 1.3分模块编译参考

### 1.3.1源码和编译环境准备

#### 1.3.1.1 源码准备

**步骤1** 下载release代码包并解压。

**步骤2** 将概述中docker容器里面路径`/usr1/openeuler/src/yocto-meta-openeuler/meta-openeuler/recipes-kernel/linux/files/config/arm64`目录下的defconfig-kernel拷贝至`mpu_solution/custom/open_euler`目录下。

#### 1.3.1.2 编译环境准备

**步骤1** 从以下路径下载openEuler-22.03-LTS-SP3交叉编译链到~/hi3093_tool目录。

[https://mirror.truenetwork.ru/openeuler/openEuler-22.03-LTS-SP3/embedded_img/aarch64/qemu-aarch64/openeuler-glibc-x86_64-openeuler-image-aarch64-qemu-aarch64-toolchain-22.03-LTS-SP3.sh](https://mirror.truenetwork.ru/openeuler/openEuler-22.03-LTS-SP3/embedded_img/aarch64/qemu-aarch64/openeuler-glibc-x86_64-openeuler-image-aarch64-qemu-aarch64-toolchain-22.03-LTS-SP3.sh)

**步骤2** 执行如下命令。

```
sh openeuler-glibc-x86_64-openeuler-image-aarch64-qemu-aarch64-toolchain-22.03-LTS-SP3.sh -d /home/${USER}/hi3093_tool/toolchain/ -y \
```

**步骤3** 配置openEuler交叉编译环境，执行如下命令
。
```
 ./home/${USER}/hi3093_tool/toolchain/environment-setup-aarch64-openeuler-linux
```

### 1.3.2 编译Hi3093 SDK

#### 1.3.2.1 Hi3093 SDK 编译准备

**步骤1** 修改编译脚本权限。

```
cd hi-mpu
chmod u+x -R build
```

**步骤2** 从社区下载openEuler-22.03-LTS-SP3对应的内核放到mpu_solution/open_source目录
下，并将目录名重命令成linux5.10，详细命令如下。

```
cd mpu_solution/open_source
wget --no-check-certificate https://gitee.com/openeuler/kernel/repository/archive/5.10.0-177.0.0.zip
unzip 5.10.0-177.0.0.zip
mv kernel-5.10.0-177.0.0/ linux5.10
```

**步骤3**	配置编译时所需要的环境。

```
cd mpu_solution/build
source build_prepare.sh
source build_exports
```

**步骤4** 编译内核：

-	更改内核默认配置
    ```shell
    cd mpu_solution
    cp custom/open_euler/defconfig-kernel open_source/linux5.10/arch/arm64/configs/defconfig
    ```
-	打patch
    ```shell
    cd mpu_solution/open_source/linux5.10
    git apply ../../src/patches/openEuler/kernel-22.03-lts-sp3-mmc.patch
    ```
-	编译内核
    ```shell
    cd mpu_solution/open_source/linux5.10
    make ARCH=arm64 defconfig
    make ARCH=arm64 CROSS_COMPILE=aarch64-openeuler-linux-gnu- -j32
    ```

#### 1.3.2.2 Hi3093 SDK 编译

-	执行如下命令编译SDK：
    ```shell
    cd mpu_solution/build/build_sdk/
    ./build_sdk.sh
    ```
-	SDK编译完成后会打包生成两个文件：hi3093sdk.tar.gz，securec.tar.gz存放在
`mpu_solution/build/output`目录下。

### 1.3.3 uboot制作

-	编译生成u-boot，需要执行如下命令。
    ```shell
    cd mpu_solution/build/build_uboot
    ./build_uboot.sh openeuler
    ```
-	命令执行完成后，在mpu_solution/build/output目录下生成u-boot_rsa_4096.bin文件。

### 1.3.4 制作uImage

用于openEuler的镜像的转换，zImage-5.10.0转换成uImage。步骤如下：

**步骤1** 执行 1.3.3 uboot制作 步骤后，将在mpu_solution/custom/open_euler目录生成mkimage可执行文件。

**步骤2** 将openEuler编译出的zImage制作成uImage，执行如下命令。

```shell
cd mpu_solution/custom/open_euler
./mkimage -A arm -O linux -T kernel -C none -a 0x98280000 -e 0x98280040 -n linux-5.10.0 -d zImage-5.10.0 uImage \
```

**步骤3** 执行完成后，在`mpu_solution/custom/open_euler`目录下生成uImage文件。

# 2. SFC镜像构建与启动

**步骤1** 构建SFC烧片包前请确保按照源码和编译环境准备准备好编译环境并按照Hi3093 SDK编译准备完成内核编译。

**步骤2** 进入build/build_sfc目录，执行build_sfc.sh脚本，在sfc_output目录中的Hi3093_flash_ext4.bin即为SFC烧片包，此时uboot读取sfc中的内核和文件系统，可用于eMMC空片时启动，此时烧片包为64M。

<span style="color:#0066cc;">须知</span>

     本文档当前仅提供SFC镜像构建方法，SFC烧写流程由用户自己完成。可以通过烧录机烧录，也可以在uboot阶段擦写flash烧录，亦可以在系统起来后通过加载flash驱动的方式重新擦写flash。

**步骤3** flash用烧录器烧写好后，将SFC颗粒放入3093单板U12对应位置的底座中，注意颗粒1脚位置，需要将颗粒上标有小圆点的角对准单板上的小圆点，否则会引起SFC短路烧毁。

**步骤4** 检查拨码开关SW1，以on侧为1，记SW1 1\~4为1110，即1\~3开关拨至on侧，4开关拨至对侧。

**步骤5** 接通电源，串口接J26，网线接J12，打开电源开关，完成从flash的启动。


# 3. SFC从emmc非安全启动

**步骤1** 将`hi-mpu/build/build_fs/init`脚本中80和81行中的nvm_active替换为目标分区（如mmcblk0p1），参考eMMC镜像构建或其它混合部署方案编译运行指南完成镜像的编译。

**步骤2** **如果当前emmc usr区未进行分区**，在uboot串口执行“env default -a”后执行“saveenv”即可设置好加载SFC镜像的环境变量，然后执行“run bootcmd”即可；**如果emmc usr区已完成分区并放好烧片包**，直接进入步骤7。

**步骤3** 输入账号密码后进入系统，执行“fdisk /dev/mmcblk0”进行分区创建，例：
- 输入“n”进行分区创建，输入“p”选择创建主分区
- 输入“1”设定分区编号为1
- 输入“144”设定该分区起始sector
- 输入“6324223”作为该分区终止sector
- 输入“w”保存当前创建操作，这里sector起止之差应大于786432，即384MB大小

分好区后，执行`mkfs.ext4 -b 4096 /dev/mmcblk0pX`（例：mkfs.ext4 -b 4096 /dev/mmcblk0p1）格式化X分区为ext4文件系统后reboot重启至内核。

**步骤4** 将编译好的烧片包hi3093_ext4.tar.gz传输至板端/tmp目录（可利用SFTP传输协议或者相关终端软件），并完成解压。

**步骤5** 执行`mkdir /mnt/update`创建镜像挂载目录，执行`mount -t ext4 -o loop /tmp/Hi3093_ext4fs.img /mnt/update/`完成镜像挂载。

**步骤6** 执行`cp -rf /mnt/update/* /run/media/mmcblk0pX`将编译好的镜像中的文件系统拷贝至X分区，并执行`reboot`重启至uboot控制台。

**步骤7** 启动emmc usr区镜像
-	执行`env delete bootargs`删除bootargs（欧拉和混合部署的bootargs使用dts中存储的）
-	执行`setenv boot_kernel_media 1`将镜像加载介质切换至emmc usr区（默认为0启动sfc镜像）
-	执行`setenv part2_user_offset Y`将emmc usr区读取的起始sector设置为想启动的usr分区的起始sector（例:mmcblk0p2起始sector设置为了6324224，这里的Y为(6324224 -144)的十六进制，为0x607f70）
-	最后执行`saveenv`保存以上环境变量并执行`run bootcmd`启动emmc usr区镜像。

**步骤8** 至步骤7已完成emmc usr区镜像加载，若想切换回加载sfc镜像，重新执行步骤3即可。