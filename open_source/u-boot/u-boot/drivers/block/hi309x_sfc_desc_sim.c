/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2019. All rights reserved.
 * Description: Hi309x special ext4 image adapter APIs
 * Create: 2018-05-10
 */
#include <common.h>
#include <blk.h>
#include <dm.h>
#include <dm/device.h>
#include <dm/lists.h>
#include <fdtdec.h>
#include <part.h>
#include <os.h>
#include <ext_common.h>
#include <hi309x_common.h>
#include <hi309x_mmc.h>
#include <env.h>
#include <command.h>

typedef enum
{
    DEVICE_FLASH = 0,
    DEVICE_EMMC = 1,
} device_type_enum;

#define UBOOT_CMD_STR_MAX_LEN 160
#define EXT4_SUPERBLOCK_START (2 * 512)

struct uboot_cmd_mgr {
    char cmd_str[UBOOT_CMD_STR_MAX_LEN];
    unsigned char probe_flag;
};
#define MAX(a, b)	(((a) > (b)) ? (a) : (b))
struct uboot_cmd_mgr g_cmd_mgr = { 0 };

/*****************************************************************************
 函 数 名  : hi309x_read_ext4img_by_boot_core
 功能描述  : 返回读取到字节数
 输入参数  : unsigned long offset: 需要读取数据的offset
           unsigned long rd_len: 读取数据长度
           void * buffer: 读取数据待存放地址的buffer
 输出参数  : 无
 返 回 值  : 读取的数据长度
*****************************************************************************/
static unsigned long hi309x_read_ext4img_by_flash(unsigned long offset, unsigned long rd_len, void *buffer)
{
    int ret = 0;
    unsigned long src_addr = EXT4_IMG_FLASH_ADDR + offset;

    if (0 == g_cmd_mgr.probe_flag) {
        ret = run_command("sf probe 0:0", 0);
        if (ret) {
            /* failed return read block cnt 0 */
            return 0;
        }
        g_cmd_mgr.probe_flag = 1;
    }
    sprintf(g_cmd_mgr.cmd_str, "sf read 0x%p 0x%x 0x%x\n", buffer, (unsigned int)src_addr, (unsigned int)rd_len);
    ret = run_command(g_cmd_mgr.cmd_str, 0);
    if (ret) {
        printf(g_cmd_mgr.cmd_str);
        printf("run read command failed\n");
        return 0;
    }
    return rd_len;
}

static int hi309x_read_ext4img_by_emmc(unsigned int blksz, unsigned long start_blk, unsigned long blkcnt, void *buffer)
{
    unsigned int offset_user = env_get_hex("part2_user_offset", 0);
    unsigned long blk_off = (start_blk * blksz + TOTAL_OFFSET_EMMC_USER) / CONFIG_ENV_SECT_SIZE + offset_user;
    unsigned long blk_num = (blkcnt * blksz) / CONFIG_ENV_SECT_SIZE;
    mmc_non_dma_change_area(0, 3);
    int ret = mmc_non_dma_read(buffer, blk_off, blk_num, 3);
    return (ret == 0) ? (blkcnt * blksz) : 0;
}

static int hi309x_write_ext4img_by_emmc(unsigned int blksz, unsigned long start_blk, unsigned long blkcnt, void *buffer)
{
    unsigned int offset_user = env_get_hex("part2_user_offset", 0);
    unsigned long blk_off = (start_blk * blksz + TOTAL_OFFSET_EMMC_USER) / CONFIG_ENV_SECT_SIZE + offset_user;
    unsigned long blk_num = blkcnt * blksz / CONFIG_ENV_SECT_SIZE;
    mmc_non_dma_change_area(0, 3);
    int ret = mmc_non_dma_write(buffer, blk_off, blk_num, 3);
    return (ret == 0) ? (blkcnt * blksz) : 0;
}

/*****************************************************************************
 函 数 名  : hi309x_block_read
 功能描述  : 返回读取到的块的个数
 输入参数  : struct udevice *dev: uboot device结构体指针
           unsigned long start_blk: 起始block号
           lbaint_t blkcnt: 读取的block数量
           void *buffer: 读取数据bufffer存放地址
 输出参数  : 无
 返 回 值  : 读取的block的数量
*****************************************************************************/
static unsigned long hi309x_block_read(struct udevice *dev, unsigned long start_blk, lbaint_t blkcnt, void *buffer)
{
    struct blk_desc *block_dev = dev_get_uclass_plat(dev);
    unsigned int boot_kernel_media = env_get_hex("boot_kernel_media", 0);
    unsigned long rd_len;
    if (boot_kernel_media == DEVICE_FLASH) {
        rd_len = hi309x_read_ext4img_by_flash(start_blk * block_dev->blksz, blkcnt * block_dev->blksz, buffer);
        return (rd_len / block_dev->blksz); /* if ok equal blkcnt */
    } else if (boot_kernel_media == DEVICE_EMMC) {
        rd_len = hi309x_read_ext4img_by_emmc(block_dev->blksz, start_blk, blkcnt, buffer);
        return (rd_len == 0) ? 0 : blkcnt;
    }
    return 0;
}

static unsigned long hi309x_block_write(struct udevice *dev, unsigned long start_blk, lbaint_t blkcnt, const void *buffer)
{
    unsigned long rd_len = 0;
    struct blk_desc *block_dev = dev_get_uclass_plat(dev);
    unsigned int boot_kernel_media = env_get_hex("boot_kernel_media", 0);
    if (boot_kernel_media == DEVICE_EMMC) {
        rd_len = hi309x_write_ext4img_by_emmc(block_dev->blksz, start_blk, blkcnt, (void *)buffer);
    }
    return (rd_len / block_dev->blksz);
}

static const struct blk_ops g_hi309x_blk_ops = {
    .read = hi309x_block_read,
    .write = hi309x_block_write,
};

/*****************************************************************************
 函 数 名  : hi309x_read_super_block
 功能描述  : 适用于blocksize大于1KB的场景,ext4 require min blksize is 1KB
 输入参数  : char *buffer:读取super block的buffer存放地址
 输出参数  : 无
 返 回 值  : 如果读取的长度和super block长度不等返回1，相等返回0代表成功
 说    明  : 无
*****************************************************************************/

#define SUPER_BLK_SIZE 1024
#define EXT4_SINGLE_BLK_SIZE 4096
static int hi309x_read_super_block(char *buffer)
{
    unsigned long rd_len;
    unsigned int boot_kernel_media = env_get_hex("boot_kernel_media", 0);
    if (boot_kernel_media == DEVICE_EMMC) {
        unsigned char tmp[SUPER_BLK_SIZE] = {0};
        int blkcnt = SUPER_BLK_SIZE / CONFIG_ENV_SECT_SIZE;
        int blk_off = (EXT4_SUPERBLOCK_START + TOTAL_OFFSET_EMMC_USER) / CONFIG_ENV_SECT_SIZE;
        int ret = mmc_non_dma_read(tmp, blk_off, blkcnt, 3); // blk_off == 146
        memcpy(buffer, tmp, sizeof(struct ext2_sblock));
        return ret;
    } else if(boot_kernel_media == DEVICE_FLASH) {
        rd_len = hi309x_read_ext4img_by_flash(EXT4_SUPERBLOCK_START, sizeof(struct ext2_sblock), buffer);
    }
    return !(rd_len == sizeof(struct ext2_sblock));
}

static int hi309x_blk_probe(struct udevice *dev)
{
    int ret;
    ulong blk_size;
    struct driver *test_drv = NULL;
    struct ext2_sblock sblock;
    struct blk_desc *desc = dev_get_uclass_plat(dev);
    unsigned int boot_kernel_media = env_get_hex("boot_kernel_media", 0);
    printf("blk_probe\n");

    ret = hi309x_read_super_block((char *)&sblock);
    if (ret) {
        printf("read superblock failed\n");
        return ret;
    }

    blk_size = (1ul << (sblock.log2_block_size + 10)); /* cal blk_size: 1 << (sblock.log2_block_size + 10) */

    printf("detect ext4img info: block cnt:%u,block size:%lu\n", sblock.total_blocks, blk_size);

    test_drv = lists_driver_lookup_name("private_device");
    desc->if_type = IF_TYPE_PRIVATE_DEVICE;
#ifdef CONFIG_HI3093_SPI
    if (boot_kernel_media == DEVICE_FLASH) {
        desc->if_type = IF_TYPE_HI3093_SPI;
    }
#endif
    desc->blksz = blk_size;
    desc->lba = sblock.total_blocks;
    desc->part_type = PART_TYPE_UNKNOWN;
    desc->bdev = dev;
    desc->devnum = 0;
    printf("log2blksz %d\n", LOG2(desc->blksz));
    desc->log2blksz = LOG2(desc->blksz);
    desc->bdev->driver = test_drv;

    return 0;
}

U_BOOT_DRIVER(private_device) = {
    .name = "private_device",
    .id = UCLASS_BLK,
    .ops = &g_hi309x_blk_ops,
    .probe = hi309x_blk_probe,
};

/*lint -e729*/
struct blk_desc g_hi309x_blk_desc;

U_BOOT_DRVINFO(blk_hi309x_non_fdt) = {
    .name = "private_device",
    .plat = &g_hi309x_blk_desc,
};
