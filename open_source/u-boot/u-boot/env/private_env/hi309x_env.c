// SPDX-License-Identifier: GPL-2.0+
/*
 * (C) Copyright 2008-2011 Freescale Semiconductor, Inc.
 */

/* #define DEBUG */

#include <common.h>
#include <command.h>
#include <env.h>
#include <env_internal.h>
#include <fdtdec.h>
#include <linux/stddef.h>
#include <malloc.h>
#include <memalign.h>
#include <hi309x_mmc.h>
#include <part.h>
#include <search.h>
#include <errno.h>

static int env_mmc_save(void)
{
	int	ret;
    ret = emmc_init();
    if (ret) {
		return -1;
    }
    u32 blk_start	= CONFIG_ENV_OFFSET / CONFIG_ENV_SECT_SIZE;
	u32 blk_cnt		= CONFIG_ENV_SIZE / CONFIG_ENV_SECT_SIZE;
    unsigned char *env_new = (unsigned char *)malloc(blk_cnt * CONFIG_ENV_SECT_SIZE);
	ret = env_export((env_t *)env_new);
	if (ret) {
		goto finish;
    }
    ret = mmc_non_dma_write(env_new, blk_start, blk_cnt, 2);
    if (ret) {
        printf("write data failed\n");
        goto finish;
    }
    printf("write env to mmc success! num %d \n", blk_cnt);
finish:
    free(env_new);
    return ret;
}

static int env_mmc_load(void)
{
	int	ret;
    ret = emmc_init();
    if (ret) {
		goto err;
    }
    u32 blk_start	= CONFIG_ENV_OFFSET / CONFIG_ENV_SECT_SIZE;
	u32 blk_cnt		= CONFIG_ENV_SIZE / CONFIG_ENV_SECT_SIZE;
    unsigned char *env_new = (unsigned char *)malloc(blk_cnt * CONFIG_ENV_SECT_SIZE);
    ret = mmc_non_dma_read(env_new, blk_start, blk_cnt, 2);
    if (ret) {
        printf("read data failed, ret = 0x%x\n", ret);
        goto err;
    }
    ret = env_import(env_new, 1, H_EXTERNAL);
err:
	if (ret) {
		env_set_default("errmsg load failed", 0);
    } else {
        printf("load env from mmc success! num %d\n", blk_cnt);
    }
    free(env_new);
    return ret;
}

U_BOOT_ENV_LOCATION(mmc) = {
	.location	= ENVL_PRIVATE_DEVICE,
	ENV_NAME("MMC")
	.load		= env_mmc_load,
	.save		= env_save_ptr(env_mmc_save),
};
