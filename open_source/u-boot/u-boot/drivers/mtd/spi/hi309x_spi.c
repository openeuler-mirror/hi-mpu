/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 */
#include <common.h>
#include <errno.h>
#include <malloc.h>
#include <fdtdec.h>
#include <spi.h>
#include <asm/io.h>
#include <hi309x_common.h>

struct hi309x_spi_slave {
    struct spi_slave slave;
    unsigned int max_hz;
    unsigned int mode;
};

typedef struct hi309x_xfer_ctrl {
    /* 读写flash的寄存器则不存在addr，当且仅当 begin len=1,主要针对flash寄存器的读写(如RDSR)，存在cmd+data
    一次完整操作完成后将其清0，即配置spi send之后，不清0 也可以，每次重新赋值 */
    u32 has_addr_rw; /* 1 读写aadren=1,0 addren=0 */
    u32 last_dummy;
    u32 addr;
    u8 cmd;
} hi309x_xfer_ctl;
/* 用于上层将一次flash操作分为多次xfer时，保存上一次xfer解析出来的参数，如读写 */
hi309x_xfer_ctl g_xfer_ctl = {0};

static inline struct hi309x_spi_slave *to_hi309x_slave(struct spi_slave *slave)
{
    if (slave == NULL) {
        return NULL;
    } else {
        return container_of(slave, struct hi309x_spi_slave, slave);
    }
}

int spi_cs_is_valid(unsigned int bus, unsigned int cs)
{
    if ((0 != bus) && (1 != bus)) {
        return 0;
    }
    if ((0 != cs) && (1 != cs)) {
        /* invalid cs */
        return 0;
    }

    return 1;
}
/* sfc iocfg*/
static void sfc_iocfg(unsigned int bus)
{
    if (bus == 0) {
        writel(0x0, IO_CONFIG_T_BASE_ADDR + IO_CONFIG_T_IOMG002_OFFSET_ADDR);
        writel(0x0, IO_CONFIG_T_BASE_ADDR + IO_CONFIG_T_IOMG003_OFFSET_ADDR);
        writel(0x0, IO_CONFIG_T_BASE_ADDR + IO_CONFIG_T_IOMG004_OFFSET_ADDR);
        writel(0x0, IO_CONFIG_T_BASE_ADDR + IO_CONFIG_T_IOMG005_OFFSET_ADDR);
    } else {
        writel(0x0, IO_CONFIG_T_BASE_ADDR + IO_CONFIG_T_IOMG006_OFFSET_ADDR);
        writel(0x0, IO_CONFIG_T_BASE_ADDR + IO_CONFIG_T_IOMG007_OFFSET_ADDR);
        writel(0x0, IO_CONFIG_T_BASE_ADDR + IO_CONFIG_T_IOMG008_OFFSET_ADDR);
        writel(0x0, IO_CONFIG_T_BASE_ADDR + IO_CONFIG_T_IOMG009_OFFSET_ADDR);
        writel(0x0, IO_CONFIG_T_BASE_ADDR + IO_CONFIG_T_IOMG010_OFFSET_ADDR);
        writel(0x0, IO_CONFIG_T_BASE_ADDR + IO_CONFIG_T_IOMG011_OFFSET_ADDR);
    }
}
struct spi_slave *spi_setup_slave(unsigned int bus, unsigned int cs, unsigned int max_hz, unsigned int mode)
{
    struct hi309x_spi_slave *hi309x_slave = NULL;

    if (!spi_cs_is_valid(bus, cs)) {
        printf("Hi309x SPI: bus %d / cs %d invalid\n", bus, cs);
        return NULL;
    }

    sfc_iocfg(bus);

    hi309x_slave = spi_alloc_slave(struct hi309x_spi_slave, bus, cs);
    if (!hi309x_slave) {
        printf("Hi309x SPI: cannot alloc hi309x_spi_slave\n");
        return NULL;
    }

    hi309x_slave->max_hz = max_hz;
    hi309x_slave->mode = mode;
    /* add 2018.06.14 */
    hi309x_slave->slave.bus = bus;
    hi309x_slave->slave.cs = cs;
    hi309x_slave->slave.max_write_size = 64; /* 控制器单次的最大大小 */

    /* Add other code */
    return &hi309x_slave->slave;
}

void spi_free_slave(struct spi_slave *slave)
{
    struct hi309x_spi_slave *hi309x_slave = to_hi309x_slave(slave);

    if (hi309x_slave == NULL) {
        return;
    }

    free(hi309x_slave);
}

int spi_claim_bus(struct spi_slave *slave)
{
    struct hi309x_spi_slave *hi309x_slave = to_hi309x_slave(slave);

    if (hi309x_slave == NULL) {
        return -1;
    }

    return 0;
}

void spi_release_bus(struct spi_slave *slave)
{
    struct hi309x_spi_slave *hi309x_slave = to_hi309x_slave(slave);

    if (hi309x_slave == NULL) {
        return;
    }
}

/*****************************************************************************
 函 数 名  : sfc_cfg_spi_cmd
 功能描述  : 通过xfer的只有命令的dout解析出cmd
 输入参数  : const u8 *cmd
 输出参数  : 无
 返 回 值  : void

 修改历史      :
  1.日    期   : 2018年5月7日
    作    者   :
    修改内容   : 新生成函数

*****************************************************************************/
static void sfc_cfg_spi_cmd(unsigned int bus, const u8 *cmd)
{
    hisfc0_write(bus, HISFC350_CMD_INS, *cmd);
}

/*****************************************************************************
 函 数 名  : sfc_cfg_spi_cmd_addr
 功能描述  : 将cmd(读写擦使能等)和地址配置到sfc控制器
 输入参数  : u8 cmd
             u32 addr
 输出参数  : 无
 返 回 值  : void
 调用函数  :
 被调函数  :

 修改历史      : 4byte的空间的访问需要通过bar选择使用
  1.日    期   : 2018年5月7日
    作    者   :
    修改内容   : 新生成函数

*****************************************************************************/
void sfc_cfg_spi_cmd_addr(unsigned int bus, u8 cmd, u32 addr)
{
    sfc_cfg_spi_cmd(bus, &cmd);
    hisfc0_write(bus, HISFC350_CMD_ADDR, (addr & HISFC350_CMD_ADDR_MASK));

    return;
}

/*****************************************************************************
 函 数 名  : up_sfc_memcpy_u8_to_u32
 功能描述  : 将8位的数据以32位为单位拷贝到32位的目标数据指针
 输入参数  : u32 *data_des
             u8 *data
             u32 len
 输出参数  : 无
 返 回 值  : uP_SFC_TEXT void
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2018年5月7日
    作    者   :
    修改内容   : 新生成函数

*****************************************************************************/
void sfc_memcpy_u8_to_u32(u32 *data_des, const u8 *data, u32 len)
{
    const u8 *pdata;
    u32 *pdata_des;
    u32 i;
    u32 tmp = 0;
    u32 loop_num;
    u32 remain;

    if ((NULL == data) || (NULL == data_des)) {
        printf("input err, data or data_des is NULL.\n");
        return;
    }

    if (0 == len) {
        printf("input err, len = 0x%x.\n", len);
        return;
    }

    pdata = data;
    pdata_des = data_des;
    loop_num = (u32)(len >> 2);
    remain = len & 0x3;

    for (i = 0; i < loop_num; i++) {
        tmp = *(u32 *)pdata;
        pdata += 4;
        *pdata_des = tmp;
        pdata_des++;
    }

    if (remain != 0) {
        memcpy((u8 *)&tmp, pdata, (unsigned long)remain);
        *pdata_des = tmp;
    }

    return;
}

/*****************************************************************************
 函 数 名  : sfc_cfg_spi_data
 功能描述  : 主要用于写的数据发送
 输入参数  : u8 *data 要发送的写数据，不是spi总线上的全部数据
             u32 len
 输出参数  : 无
 返 回 值  : void
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2018年5月7日
    作    者   :
    修改内容   : 新生成函数

*****************************************************************************/
void sfc_cfg_spi_data(u32 bus, const u8 *data, u32 len)
{
    u8 *sfc_reg_base = (u8 *)CONFIG_HISFC350_REG_BASE_ADDRESS;

    if (SFC_BUS_0 == bus) {
        sfc_reg_base = (u8 *)HISFC0_REG_BASE_ADDRESS;
    }
    sfc_memcpy_u8_to_u32((u32 *)(sfc_reg_base + HISFC350_CMD_DATABUF0), data, len);

    return;
}

/*****************************************************************************
 函 数 名  : sfc_get_spi_data
 功能描述  : 用于获取读取的数据
 输入参数  : u8 *data
             u32 len
 输出参数  : 无
 返 回 值  : void
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2018年5月7日
    作    者   :
    修改内容   : 新生成函数

*****************************************************************************/
void sfc_get_spi_data(u32 bus, u8 *data, u32 len)
{
    unsigned int numread;
    int index = 0;
    unsigned char *ptr = data;

    /* 大于等于四字节数据的读取 */
    numread = (len & ~0x03);
    while (numread) {
        *(unsigned int *)ptr = hisfc0_read(bus, HISFC350_CMD_DATABUF0 + index);
        ptr += 4;
        index += 4;
        numread -= 4;
    }
    /* 小于四字节数据的读取 */
    numread = (len & 0x03);
    if (numread) {
        u32 tmp = 0;
        tmp = hisfc0_read(bus, HISFC350_CMD_DATABUF0 + index);
        memcpy(ptr, &tmp, numread);
    }

    return;
}

/*****************************************************************************
 函 数 名  : sfc_cmd_wait_cpu_finsh
 功能描述  : 等待控制器操作完成
 输出参数  : 无
 返 回 值  : int
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2018年5月7日
    作    者   :
    修改内容   : 新生成函数

*****************************************************************************/
int sfc_cmd_wait_cpu_finsh(u32 bus)
{
    do {
        unsigned int timeout = 0x10000000;
        while (((hisfc0_read(bus, HISFC350_CMD_CONFIG) & HISFC350_CMD_CONFIG_START)) && timeout) {
            --timeout;
        }
        if (!timeout) {
            printf("bus 0x%x cmd wait cpu finish timeout\n", bus);
            return -EBUSY;
        }
    } while (0);

    return 0;
}

/*****************************************************************************
 函 数 名  : spi_general_write_enable
 功能描述  : flash通用的写使能操作
 输入参数  : struct hisfc_spi *spi
 输出参数  : 无
 返 回 值  : int 成功返回0，失败返回其它错误码
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2018年5月3日
    作    者   :
    修改内容   : 新生成函数

*****************************************************************************/
int spi_flash_general_write_enable(u32 bus, u32 cs)
{
    unsigned int regval;

    hisfc0_write(bus, HISFC350_CMD_INS, SPI_CMD_WREN);

    regval = HISFC350_CMD_CONFIG_SEL_CS(cs) | HISFC350_CMD_CONFIG_START;
    hisfc0_write(bus, HISFC350_CMD_CONFIG, regval);

    return sfc_cmd_wait_cpu_finsh(bus);
}

/*****************************************************************************
 函 数 名  : hisfc350_set_host_addr_mode
 功能描述  : 配置sfc0的控制寄存器的地址模式bit
 输入参数  : struct hisfc_host *host_s
             int enable 真:四字节地址模式，0:3字节地址模式
 输出参数  : 无
 返 回 值  : static void
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2018年5月2日
    作    者   :
    修改内容   : 新生成函数

*****************************************************************************/
static void hisfc350_set_host_addr_mode(u32 bus, int enable)
{
    unsigned int regval;

    regval = hisfc0_read(bus, HISFC350_GLOBAL_CONFIG);
    if (enable) {
        regval |= HISFC350_GLOBAL_CONFIG_ADDR_MODE_4B;
    } else {
        regval &= ~HISFC350_GLOBAL_CONFIG_ADDR_MODE_4B;
    }

    hisfc0_write(bus, HISFC350_GLOBAL_CONFIG, regval);

    return;
}

/*****************************************************************************
 函 数 名  : sfc_flash_addr_mode_set
 功能描述  : 控制器和flash的地址模式切换，暂未使用，框架不支持4byte
 输入参数  : u32 cs
             u32 addrcycle
             u32 enable
 输出参数  : 无
 返 回 值  : void
 调用函数  :
 被调函数  :

 修改历史      : 4byte空间通过wbar操作选择，(micron: cmd 0xc5 0xc8)
  1.日    期   : 2018年5月7日
    作    者   :
    修改内容   : 新生成函数

*****************************************************************************/
void sfc_flash_addr_mode_set(u32 bus, u32 cs, u32 addrcycle, u32 enable)
{
    /* 强制等于4 */
    if (addrcycle != 4) {
        return;
    }

    if (enable) {
        (void)spi_flash_general_write_enable(bus, cs);
        hisfc0_write(bus, HISFC350_CMD_INS, SPI_CMD_EN4B);
    } else {
        (void)spi_flash_general_write_enable(bus, cs);
        hisfc0_write(bus, HISFC350_CMD_INS, SPI_CMD_EX4B);
    }

    hisfc0_write(bus, HISFC350_CMD_CONFIG, HISFC350_CMD_CONFIG_SEL_CS(cs) | HISFC350_CMD_CONFIG_START);

    (void)sfc_cmd_wait_cpu_finsh(bus);

    hisfc350_set_host_addr_mode(bus, enable);

    return;
}

/*****************************************************************************
 函 数 名  : sfc_cfg_spi_send
 功能描述  : 配置控制器开启数据发送
 输入参数  : u32 cs
             u32 if_type 0
             u32 dummy 写擦为0，读根据参数填充
             u32 addren
             u32 dataen
             u32 data_cnt
             u32 rw 默认为0  擦除和写均填0 ，读填1
 输出参数  : 无
 返 回 值  : 成功返回0
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2018年5月7日
    作    者   :
    修改内容   : 新生成函数

*****************************************************************************/
int sfc_cfg_spi_send(u32 bus, u32 cs, u32 if_type, u32 dummy, u32 addren, u32 dataen, u32 data_cnt, u32 rw)
{
    int ret = 0;

    u32 reg_val = HISFC350_CMD_CONFIG_SEL_CS(cs) | HISFC350_CMD_CONFIG_MEM_IF_TYPE(if_type) |
                  HISFC350_CMD_CONFIG_DUMMY_CNT(dummy) | HISFC350_CMD_CONFIG_START | rw;

    /* 暂不需要配置，框架已通过wr bar进行配置 */
    if (addren) {
        reg_val |= HISFC350_CMD_CONFIG_ADDR_EN;
    }

    if (dataen) {
        reg_val |= HISFC350_CMD_CONFIG_DATA_CNT(data_cnt);
        reg_val |= HISFC350_CMD_CONFIG_DATA_EN;
    }

    hisfc0_write(bus, HISFC350_CMD_CONFIG, reg_val);
    ret = sfc_cmd_wait_cpu_finsh(bus);

    return ret;
}

/*****************************************************************************
 函 数 名  : get_cmd_and_addr
 功能描述  : 通过xfer的4个byte的dout解析出3byte的地址和1byte的cmd
 输入参数  : const u8 *dout
             u32 dout_byte_len
             u8 *cmd
             u32 *addr
 输出参数  : 无
 返 回 值  : int
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2018年5月7日
    作    者   :
    修改内容   : 新生成函数

*****************************************************************************/
int get_cmd_and_addr(struct spi_slave *slave, const u8 *dout, u32 dout_byte_len, u8 *cmd, u32 *addr)
{
    u8 cmdsz = SPI_FLASH_CMD_LEN;

    if (slave->spec_para.use_4b_flag) {
        cmdsz = SPI_FLASH_4B_CMD_LEN;
    }

    if (dout_byte_len < cmdsz) {
        return -EINVAL;
    }

    if (slave->spec_para.use_4b_flag) {
        *addr = (dout[1] << 24) | (dout[2] << 16) | (dout[3] << 8) | (dout[4] << 0);
    } else {
        *addr = (dout[1] << 16) | (dout[2] << 8) | (dout[3] << 0);
    }

    *cmd = dout[0];

    return 0;
}

/*****************************************************************************
 函 数 名  : sfc_reg_write_buf
 功能描述  : 下发一次写指令，即len小于64
 输入参数  : u8 * data
             u32 if_type
             u32 len
             u32 cs
 输出参数  : 无
 返 回 值  : int
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2018年5月7日
    作    者   :
    修改内容   : 新生成函数

*****************************************************************************/
int sfc_reg_write_buf(const u8 *data, u32 if_type, u32 addr, u32 len, u32 bus, u32 cs)
{
    int ret = 0;
    u32 addr_en = g_xfer_ctl.has_addr_rw;  // 1;
    u32 data_en = 1;
    u32 rw = 0;

    /* 根据写数据的长度配置data */
    sfc_cfg_spi_data(bus, data, len);
    sfc_cfg_spi_cmd(bus, &(g_xfer_ctl.cmd));
    hisfc0_write(bus, HISFC350_CMD_ADDR, (addr & HISFC350_CMD_ADDR_MASK));

    /* 配置CMD配置寄存器 */
    ret = sfc_cfg_spi_send(bus, cs, if_type, 0, addr_en, data_en, len, rw);

    return ret;
}

/*****************************************************************************
 函 数 名  : hisfc350_reg_write
 功能描述  : 写Flash存储内容
 输入参数  : u32 addr
             u8* data
             u32 len
 输出参数  : 无
 返 回 值  : 成功返回ERR_OK，失败返回其他错误码
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2018年6月4日
    作    者   :
    修改内容   : 新生成函数

*****************************************************************************/
int sfc_reg_write(const u8 *data, u32 if_type, u32 addr, u32 len, u32 bus, u32 cs)
{
    int ret = 0;
    u32 page_bottom, beyond_remain;
    u8 *data_temp;
    u32 addr_temp;

    data_temp = (u8 *)data;
    addr_temp = addr;
    /* 与页底的距离 */
    page_bottom = SPI_FLASH_PAGE_SIZE - (addr % SPI_FLASH_PAGE_SIZE);
    /* 避免跨页先写余数，之后都能被64整除 */
    beyond_remain = page_bottom % HISFC350_REG_BUF_SIZE;

    if (len >= beyond_remain) {
        if (beyond_remain != 0) { /* 保证不会在跨页 */
            ret = sfc_reg_write_buf(data_temp, if_type, addr_temp, beyond_remain, bus, cs);
            if (ret != 0) {
                printf("sfc reg write buf fail(0x%x)!\n", addr_temp);
                return ret;
            }
            addr_temp += beyond_remain;
            data_temp += beyond_remain;
            len -= beyond_remain;
        }
    }

    while (len >= HISFC350_REG_BUF_SIZE) {
        ret = sfc_reg_write_buf(data_temp, if_type, addr_temp, HISFC350_REG_BUF_SIZE, bus, cs);
        if (ret != 0) {
            printf("sfc_reg_write_buf fail(0x%x)!\n", addr_temp);
            return ret;
        }
        addr_temp += HISFC350_REG_BUF_SIZE;
        data_temp += HISFC350_REG_BUF_SIZE;
        len -= HISFC350_REG_BUF_SIZE;
    }

    if (len) {
        ret = sfc_reg_write_buf(data_temp, if_type, addr_temp, len, bus, cs);
        if (ret != 0) {
            printf("sfc reg write buf fail(0x%x) len 0x%x!\n", addr_temp, len);
            return ret;
        }
        /* 已完成读操作，变量addr_temp，data_temp不用再增加 */
    }

    return ret;
}

/*****************************************************************************
 函 数 名  : sfc_reg_read_buf
 功能描述  : 进行一次读取操作
 输入参数  : u8 *data
             u32 len
             u32 if_type
             u32 dummy
             u32 cs
 输出参数  : 无
 返 回 值  : int
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2018年5月7日
    作    者   :
    修改内容   : 新生成函数

*****************************************************************************/
int sfc_reg_read_buf(u8 *data, u32 len, u32 addr, u32 if_type, u32 dummy, u32 bus, u32 cs)
{
    int ret = 0;
    u32 addr_en = g_xfer_ctl.has_addr_rw;  // 1;
    u32 data_en = 1;
    u32 data_cnt = len;
    u32 rw = HISFC350_CMD_CONFIG_RW_READ;
    hisfc0_write(bus, HISFC350_CMD_ADDR, (addr & HISFC350_CMD_ADDR_MASK));
    sfc_cfg_spi_cmd(bus, &(g_xfer_ctl.cmd));
    ret = sfc_cfg_spi_send(bus, cs, if_type, dummy, addr_en, data_en, data_cnt, rw);
    sfc_get_spi_data(bus, data, data_cnt);

    return ret;
}

/*****************************************************************************
 函 数 名  : hisfc350_reg_read
 功能描述  : 寄存器方式读取flash
 输入参数  : struct mtd_info *mtd
             loff_t from 读取偏移
             size_t len 读取长度
             size_t *retlen 实际读取长度
             u_char *buf 数据
 输出参数  : 无
 返 回 值  : static int 成功返回0，失败返回其它错误码
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2018年5月3日
    作    者   :
    修改内容   : 新生成函数

*****************************************************************************/
static int sfc_reg_read(u8 *data, u32 addr, u32 len, u32 if_type, u32 dummy, u32 bus, u32 cs)
{
    int num;
    int ret = 0;
    u8 *ptr = data;

    while (len > 0) {
        num = len;
        while (num >= HISFC350_REG_BUF_SIZE) {
            ret = sfc_reg_read_buf(ptr, HISFC350_REG_BUF_SIZE, addr, if_type, dummy, bus, cs);
            if (ret) {
                printf("sfc_reg_read_buf err addr 0x%x", addr);
                return ret;
            }
            ptr += HISFC350_REG_BUF_SIZE;
            addr += HISFC350_REG_BUF_SIZE;
            len -= HISFC350_REG_BUF_SIZE;
            num -= HISFC350_REG_BUF_SIZE;
        }

        if (num) {
            ret = sfc_reg_read_buf(ptr, num, addr, if_type, dummy, bus, cs);
            if (ret) {
                printf("sfc_reg_read_buf err addr 0x%x, num 0x%x", addr, num);
                return ret;
            }
            addr += num;
            ptr += num;
            len -= (unsigned int)num;
        }
    }

    return ret;
}

/*****************************************************************************
 函 数 名  : spi_xfer
 功能描述  : 1
 输入参数  : struct spi_slave *slave
             unsigned int bitlen:length in bits
             const void *dout data out,指示spi总线发送的数据包括dummy
             void *din 指示读取数据的fifo
             unsigned long flags: see include/spi.h
 输出参数  : 无
 返 回 值  : int
 调用函数  :
 被调函数  :
 说明      : 代码分析:dout是spi总线上要发送的数据包括命令、地址、dummy、以及传输的数据;
             1、当flag仅包含begin，则dout是cmd(1)+[addr(3)]+[dummy(>=0)]，
             此时只需要配置相关参数，不要触发发送指令;
             exam:
             (1)wren:dout=1cmd,cmdlen=1;
             (2)write/erase:dout=1cmd+3addr,cmdlen=4;
             (3)read:dout=1cmd+3addr+ndummybyte,cmdlen=4+dummybyte
             2、当flag同时包含begin和end，dout解释如上，配置完参数还需要触发发送指令;
             3、当flag仅包含end，表示dout或din是要向buf填充或从buf读取的数据，
             只需做完相关操作触发控制器执行进行数据的发送或读取，相关的参数框架已经在
             紧邻的上一次操作中进行配置
             4、对于长度超过页长的拆分读取已由框架完成，此处无需处理
             5、框架不支持4byte地址模式，对于超过3byte空间使用wbar进行选择;
             需要配置:CONFIG_SPI_FLASH_BAR
 修改历史      :
  1.日    期   : 2018年5月7日
    作    者   :
    修改内容   : 新生成函数

*****************************************************************************/
int spi_xfer(struct spi_slave *slave, unsigned int bitlen, const void *dout, void *din, unsigned long flags)
{
    u8 cmd = 0;
    u32 addr = 0;
    const u8 *txp = NULL;
    unsigned int len;
    int ret = 0;
    u32 rw = 0;
    u32 bus = slave->bus;
    u32 cs_val = slave->cs;
    u32 if_type = 0; /* 动态配置 */
    u32 dummy_byte = 0;
    u32 addr_en = 0;
    u32 data_en = 0;
    u32 data_cnt = 0;
    u8 cmdsz = SPI_FLASH_CMD_LEN;

    txp = dout;

    if (bitlen == 0) { /* Finish any previously submitted transfers */
        printf("bitlen is 0\n");
        ret = -EINVAL;
        goto out;
    }

    /*
     * TODO: The controller can do non-multiple-of-8 bit
     * transfers, but this driver currently doesn't support it.
     *
     * It's also not clear how such transfers are supposed to be
     * represented as a stream of bytes...this is a limitation of
     * the current SPI interface.
     */
    if (bitlen % 8) {
        /* Errors always terminate an ongoing transfer */
        printf("bitlen 0x%x div 8 not 0\n", bitlen);
        ret = -EINVAL;
        goto out;
    }

    len = bitlen / 8;

    /* 读写擦除外层已经按照sector自动分次调用该函数  此处不需要关心 */
    if (flags & SPI_XFER_BEGIN) { /* 擦除或纯cmd的情形 */
        if (NULL == dout) {
            printf("dout is null\n");
            return -EINVAL;
        }
        /* 只有cmd 没有地址 */
        if (len == 1) {
            sfc_cfg_spi_cmd(bus, txp);
            g_xfer_ctl.has_addr_rw = 0;
            g_xfer_ctl.last_dummy = 0;
            g_xfer_ctl.cmd = *txp;
            addr_en = 0;
            data_en = 0;
            data_cnt = 0;
        } else {
            ret = get_cmd_and_addr(slave, txp, len, &cmd, &addr);
            if (ret) {
                printf("get_cmd_and_addr err, len 0x%x\n", len);
                return ret;
            }
            sfc_cfg_spi_cmd_addr(bus, cmd, addr);
            g_xfer_ctl.has_addr_rw = 1;
            g_xfer_ctl.addr = addr;
            g_xfer_ctl.cmd = cmd;
            addr_en = 1;
            data_en = 0;
            data_cnt = 0;
            if (slave->spec_para.use_4b_flag) {
                cmdsz = SPI_FLASH_4B_CMD_LEN;
            }

            if (len > cmdsz) {
                dummy_byte = len - cmdsz;
                g_xfer_ctl.last_dummy = dummy_byte;
            }
        }

        /* start和end同时有无需发送数据 */
        if (flags & SPI_XFER_END) {
            ret = sfc_cfg_spi_send(bus, cs_val, if_type, dummy_byte, addr_en, data_en, data_cnt, rw);
        }
    } else if (flags & SPI_XFER_END) { /* 只有end 需要根据null的情况将数据发送并读取，读或者写的情形 */
        /* 这里dout不包含cmd和地址，上层已经根据是否有数据调用该函数2次了 */
        /* 走到这里dout和din必有一个不为null */
        if (dout != NULL) { /* 写 需要发送数据 */
            ret = sfc_reg_write(txp, if_type, g_xfer_ctl.addr, len, bus, cs_val);
            if (ret) {
                printf("sfc_reg_write_buf err, len 0x%x,cs 0x%x\n", len, cs_val);
            }
        } else if (din != NULL) { /* 读 需要读取数据 */
            ret = sfc_reg_read(din, g_xfer_ctl.addr, len, if_type, g_xfer_ctl.last_dummy, bus, cs_val);
            if (ret) {
                printf("sfc_reg_write_buf err, len 0x%x,cs 0x%x, dummy 0x%x\n", len, cs_val, g_xfer_ctl.last_dummy);
            }
        }
    }

    if (flags & SPI_XFER_END) {
        g_xfer_ctl.has_addr_rw = 0;
        g_xfer_ctl.last_dummy = 0;
        g_xfer_ctl.addr = 0;
        g_xfer_ctl.cmd = 0;
    }

    return ret;
out:
    (void)sfc_cmd_wait_cpu_finsh(bus);

    return ret;
}
static int spi_flash_read_write(
    struct spi_slave *spi, const u8 *cmd, size_t cmd_len, const u8 *data_out, u8 *data_in, size_t data_len)
{
    unsigned long flags = SPI_XFER_BEGIN;
    int ret;

    if (data_len == 0)
        flags |= SPI_XFER_END;

    ret = spi_xfer(spi, cmd_len * 8, cmd, NULL, flags);
    if (ret) {
        debug("SF: Failed to send command (%zu bytes): %d\n", cmd_len, ret);
    } else if (data_len != 0) {
        ret = spi_xfer(spi, data_len * 8, data_out, data_in, SPI_XFER_END);
        if (ret) {
            debug("SF: Failed to transfer %zu bytes of data: %d\n", data_len, ret);
        }
    }

    return ret;
}

int spi_flash_cmd_read(struct spi_slave *spi, const u8 *cmd, size_t cmd_len, void *data, size_t data_len)
{
    return spi_flash_read_write(spi, cmd, cmd_len, NULL, data, data_len);
}

int spi_flash_cmd(struct spi_slave *spi, u8 cmd, void *response, size_t len)
{
    return spi_flash_cmd_read(spi, &cmd, 1, response, len);
}

int spi_flash_cmd_write(struct spi_slave *spi, const u8 *cmd, size_t cmd_len, const void *data, size_t data_len)
{
    return spi_flash_read_write(spi, cmd, cmd_len, data, NULL, data_len);
}
