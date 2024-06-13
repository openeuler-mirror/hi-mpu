/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 */
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include "emmc_divide.h"
static void emmc_gpp_divide_help(void)
{
    printf("#########################################################\n");
    printf("#  e.g: emmc_divide 384\n");
    printf("#   or: emmc_divide    \n");
    printf("#########################################################\n");
}

static bool isdigitstr(char *str)
{
    return (strspn(str, "0123456789") == strlen(str));
}

/*****************************************************************************
 函 数 名  : exec_emmc_gpp_cmd
 功能描述  :gpp操作处理函数
 输入参数  : int argc
             char* argv[]
 输出参数  : 无
 返 回 值  : 无

 修改历史      :
  1.日    期   : 2019年6月28日
    修改内容   : 新生成函数

*****************************************************************************/
static int exec_emmc_gpp_cmd(int argc, char *argv[])
{
    int ret = 0;
    int fd;
    unsigned int gpp_size_in = 0;
    unsigned int gpp_size[4] = { 0 };
    int argv1 = 0;

    if (argc > NUM2) {
        emmc_gpp_divide_help();
        return CMD_ERR;
    }
    if (argv[1] == NULL) {
        gpp_size_in = SET_GPP0_SIZE;
    } else if (isdigitstr(argv[1])) {
        argv1 = atoi(argv[1]);
        if (argv1 > 0) {
            gpp_size_in = argv1 * BIT10;
        }
    } else {
        emmc_gpp_divide_help();
        return CMD_ERR;
    }
    gpp_size[0] = gpp_size_in;    // 0:gpp0大小
    gpp_size[1] = gpp_size_in;    // 1:gpp1大小
    gpp_size[2] = gpp_size_in;    // 2:gpp2大小
    gpp_size[3] = SET_GPP3_SIZE;  // 3:gpp3大小

    fd = open("/dev/mmc", O_RDONLY);
    if (fd < 0) {
        printf("open mmc failed\n");
        return CMD_ERR;
    }

    ret = ioctl(fd, MMC_GPP_DIV, gpp_size);
    if (ret < 0) {
        printf("GPP partition failed\n");
    } else {
        printf("GPP partition success\n");
    }

    close(fd);

    return ret;
}

int main(int argc, char *argv[])
{
    int ret = 0;
    ret = exec_emmc_gpp_cmd(argc, argv);
    if (ret < 0) {
        return -1;
    }
    return 0;
}