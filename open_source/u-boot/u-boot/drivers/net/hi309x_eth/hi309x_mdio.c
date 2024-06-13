/*
* Copyright (c) Huawei Technologies Co., Ltd. 2018-2020. All rights reserved.
* Description: none
* Create: 2018-08-01
* Notes: none
*/

#include "hi309x_mdio.h"
#include <command.h>
#include <common.h>
#include <config.h>
#include <linux/delay.h>
#include "hi309x_eth.h"

u32 MDIO_ReadyGet(u32 ulMdioId)
{
    MDIO_CMD_U unMdioCmd;

    if (ulMdioId >= MDIO_MAX_DRV_NUM) {
        return (u32)-1;
    }

    MAC_REG_READ(ulMdioId, MDIO_CMD_REG, unMdioCmd.u32);
    return (unMdioCmd.bits.mdio_go);
}

/* ****************************************************************************
 函 数 名  : MDIO_ReadyWait
 功能描述  : 等待mdio接口操作完成
 输入参数  : u32 ulMdioId
 输出参数  : 无
 返 回 值  : ulTimeoutUS
**************************************************************************** */
u32 MDIO_ReadyWait(u32 ulMdioId)
{
    u32 ulTimeoutUS = MDIO_WAIT_READY_CNT;

    while (--ulTimeoutUS && (MDIO_ReadyGet(ulMdioId))) {
        udelay(10); /* delay 10us */
    }

    return ulTimeoutUS;
}

/* ****************************************************************************
 函 数 名  : MDIO_StartPhyRead
 功能描述  : 执行读操作
 输入参数  : u32 ulMdioId
             u32 ulPhyAddr
             u32 ulRegNum
 输出参数  : 无
 返 回 值  : 无
**************************************************************************** */
void MDIO_StartPhyRead(u32 ulMdioId, u32 ulPhyAddr, u32 ulRegNum)
{
    MDIO_CMD_U unMdioCmd;
    unMdioCmd.u32 = 0;
    MDIO_REG_READ(ulMdioId, MDIO_CMD_REG, unMdioCmd.u32);
    unMdioCmd.bits.mdio_st = 1;
    unMdioCmd.bits.mdio_go = 1;
    unMdioCmd.bits.op_code = 0x2;
    unMdioCmd.bits.phy_addr = ulPhyAddr;
    unMdioCmd.bits.reg_addr = ulRegNum;
    MDIO_REG_WRITE(ulMdioId, MDIO_CMD_REG, unMdioCmd.u32);
}

/* ****************************************************************************
 函 数 名  : MDIO_GetPhyReadVal
 功能描述  : 取读操作的结果
 输入参数  : u32 ulMdioId
 输出参数  : 无
 返 回 值  : PHY寄存器的值
**************************************************************************** */
u32 MDIO_GetPhyReadVal(u32 ulMdioId)
{
    MDIO_RD_WR_DATA_U unReadData;

    MDIO_REG_READ(ulMdioId, MDIO_RDATA_REG, unReadData.u32);
    mdelay(5); /* delay 5ms */
    return (unReadData.bits.mdio_wr_data);
}

/* ****************************************************************************
 函 数 名  : MDIO_PhyWrite
 功能描述  : 执行mdio写操作
 输入参数  : u32 ulMdioId
             u32 ulPhyAddr
             u32 ulRegnum
             u32 ulVal
 输出参数  : 无
 返 回 值  : 无
**************************************************************************** */
void MDIO_PhyWrite(u32 ulMdioId, u32 ulPhyAddr, u32 ulRegnum, u32 ulVal)
{
    MDIO_CMD_U unMdioCmd;
    MDIO_RD_WR_DATA_U unWriteData;
    unWriteData.u32 = 0;
    unMdioCmd.u32 = 0;

    unWriteData.bits.mdio_wr_data = ulVal;
    MDIO_REG_WRITE(ulMdioId, MDIO_WDATA_REG, unWriteData.u32);
    MDIO_REG_READ(ulMdioId, MDIO_CMD_REG, unMdioCmd.u32);
    unMdioCmd.bits.mdio_st = 1;
    unMdioCmd.bits.mdio_go = 1;
    unMdioCmd.bits.op_code = 1;
    unMdioCmd.bits.phy_addr = ulPhyAddr;
    unMdioCmd.bits.reg_addr = ulRegnum;
    MDIO_REG_WRITE(ulMdioId, MDIO_CMD_REG, unMdioCmd.u32);
}

/* ****************************************************************************
 函 数 名  : MDIO_Init
 功能描述  : MDIO接口的初始化
 输入参数  : u32 ulMdioId
 输出参数  : 无
 返 回 值  : OK / ERROR
**************************************************************************** */
s32 hi309x_mdio_init(u32 mdio_id)
{
    MDIO_CMD_U unMdioCmd;

    if (mdio_id >= MDIO_MAX_DRV_NUM) {
        return -1;
    }

    /* MDIO IO复用 */
    REG_WRITE(IO_CONFIG_T_IOMG033_ADDR, 0x0);

    /* 初始化MDIO接口时钟频率为2.5M */
    unMdioCmd.u32 = 0;
    unMdioCmd.bits.mdio_clk_sel = 1;
    MDIO_REG_WRITE(mdio_id, MDIO_CMD_REG, unMdioCmd.u32);

    return 0;
}

/* ****************************************************************************
 函 数 名  : hi309x_mdio_read
 功能描述  : MDIO接口的读操作函数
 输入参数  : u32 ulMdioId
             u32 ulPhyAddr
             u32 ulRegAddr
             u32* pulValue
 输出参数  : 无
 返 回 值  : OK / ERROR
**************************************************************************** */
s32 hi309x_mdio_read(u32 ulMdioId, u32 ulPhyAddr, u32 ulRegAddr, u32 *pulValue)
{
    if (ulMdioId >= MDIO_MAX_DRV_NUM) {
        printf("ulMdioId must less than 1\n");
        return -1;
    }

    if (ulPhyAddr > MDIO_MAX_PHY_ADDR) {
        printf("read phyaddr must less than 0x1f\n");
        return -1;
    }

    if (ulRegAddr > MDIO_MAX_PHY_REG_ADDR) {
        printf("read regaddr must less than 0x30\n");
        return -1;
    }

    if (!pulValue) {
        printf("pulValue null .\n");
        return -1;
    }

    if (!MDIO_ReadyWait(ulMdioId)) {
        printf("MDIO_ReadyWait 2 .\n");
        return -1;
    }

    MDIO_StartPhyRead(ulMdioId, ulPhyAddr, ulRegAddr);

    if (MDIO_ReadyWait(ulMdioId)) {
        mdelay(1);
        *pulValue = MDIO_GetPhyReadVal(ulMdioId);
    } else {
        printf("MDIO_ReadyWait 3 .\n");
        return -1;
    }

    return 0;
}

/* ****************************************************************************
 函 数 名  : hi309x_mdio_write
 功能描述  : MDIO接口的写操作函数
 输入参数  : u32 ulMdioId
             u32 ulPhyAddr
             u32 ulRegAddr
             u32 ulValue
 输出参数  : 无
 返 回 值  : OK / ERROR
**************************************************************************** */
s32 hi309x_mdio_write(u32 ulMdioId, u32 ulPhyAddr, u32 ulRegAddr, u32 ulValue)
{
    if (ulMdioId >= MDIO_MAX_DRV_NUM) {
        printf("ulMdioId must less than 2\n");
        return -1;
    }

    if (ulPhyAddr > MDIO_MAX_PHY_ADDR) {
        printf("write phyaddr must less than 0x1f\n");
        return -1;
    }

    if (ulRegAddr > MDIO_MAX_PHY_REG_ADDR) {
        printf("write regaddr must less than 0x30\n");
        return -1;
    }

    if (!MDIO_ReadyWait(ulMdioId)) {
        printf("MDIO_ReadyWait1 .\n");
        return -1;
    }
    MDIO_PhyWrite(ulMdioId, ulPhyAddr, ulRegAddr, ulValue);
    mdelay(5); /* delay 5ms */

    return OK;
}

int do_phyread(struct cmd_tbl*cmdtp, int flag, int argc, char * const argv[])
{
    u32 phyaddr;
    u32 regaddr;
    u32 phyread = 0;

    phyaddr = simple_strtoul(argv[1], NULL, 16); /* 16进制读取参数arg 1 */
    regaddr = simple_strtoul(argv[2], NULL, 16); /* 16进制读取参数arg 2 */
    if (hi309x_mdio_read(0, phyaddr, regaddr, &phyread) != 0) {
        printf("read error\n");
    }
    printf("read phy register %d : 0x%x\n", regaddr, phyread);
    return 0;
}
U_BOOT_CMD(phyread, 3, 1, do_phyread, "read value of phy register", "");

int do_phywrite(struct cmd_tbl*cmdtp, int flag, int argc, char * const argv[])
{
    u32 phyaddr;
    u32 regaddr;
    u32 wirtevalue;
    u32 phyread = 0;
    phyaddr = simple_strtoul(argv[1], NULL, 16); /* 16进制读取参数arg 1 */
    regaddr = simple_strtoul(argv[2], NULL, 16); /* 16进制读取参数arg 2 */
    wirtevalue = simple_strtoul(argv[3], NULL, 16); /* 16进制读取参数arg 3 */
    printf("phyaddr = %d, regaddr = %d, writevalue = 0x%08X\n", phyaddr, regaddr, wirtevalue);
    if (hi309x_mdio_write(0, phyaddr, regaddr, wirtevalue) != 0) {
        printf("wirte error\n");
    }
    mdelay(1);
    hi309x_mdio_read(0, phyaddr, regaddr, &phyread);
    printf("read phy register %d : 0x%x\n", regaddr, phyread);
    return 0;
}

U_BOOT_CMD(phywrite, 4, 1, do_phywrite, "write value to phy register", "");
