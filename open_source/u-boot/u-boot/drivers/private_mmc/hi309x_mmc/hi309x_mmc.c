#include "hi309x_mmc.h"
#include "hisi_mmc.h"
#include "emmc_addr_define.h"

struct emmc_host g_host = { 0 };
struct emmc_des g_dma_desc[MAX_DMA_DESC_NUM] = { 0 };
unsigned int f_clk[4] = {400000,300000,200000,100000};

#define IO_CONFIG_R_BASE_ADDR            0x876e000
#define IO_CONFIG_R_IOMG007_OFFSET_ADDR  0x1C
#define IO_CONFIG_R_IOMG008_OFFSET_ADDR  0x20
#define IO_CONFIG_R_IOMG009_OFFSET_ADDR  0x24
#define IO_CONFIG_R_IOMG010_OFFSET_ADDR  0x28
#define IO_CONFIG_R_IOMG011_OFFSET_ADDR  0x2C
#define IO_CONFIG_R_IOMG012_OFFSET_ADDR  0x30
#define IO_CONFIG_R_IOMG013_OFFSET_ADDR  0x34
#define IO_CONFIG_R_IOMG014_OFFSET_ADDR  0x38
#define IO_CONFIG_R_IOMG015_OFFSET_ADDR  0x3C
#define IO_CONFIG_R_IOMG016_OFFSET_ADDR  0x40

#if defined(CONFIG_HI3093_SPI_BOOT)
static void mmc_set_io_cfg(void)
{
    writel(0x0, IO_CONFIG_R_BASE_ADDR + IO_CONFIG_R_IOMG007_OFFSET_ADDR);
    writel(0x0, IO_CONFIG_R_BASE_ADDR + IO_CONFIG_R_IOMG008_OFFSET_ADDR);
    writel(0x0, IO_CONFIG_R_BASE_ADDR + IO_CONFIG_R_IOMG009_OFFSET_ADDR);
    writel(0x0, IO_CONFIG_R_BASE_ADDR + IO_CONFIG_R_IOMG010_OFFSET_ADDR);
    writel(0x0, IO_CONFIG_R_BASE_ADDR + IO_CONFIG_R_IOMG011_OFFSET_ADDR);
    writel(0x0, IO_CONFIG_R_BASE_ADDR + IO_CONFIG_R_IOMG012_OFFSET_ADDR);
    writel(0x0, IO_CONFIG_R_BASE_ADDR + IO_CONFIG_R_IOMG013_OFFSET_ADDR);
    writel(0x0, IO_CONFIG_R_BASE_ADDR + IO_CONFIG_R_IOMG014_OFFSET_ADDR);
    writel(0x0, IO_CONFIG_R_BASE_ADDR + IO_CONFIG_R_IOMG015_OFFSET_ADDR);
    writel(0x0, IO_CONFIG_R_BASE_ADDR + IO_CONFIG_R_IOMG016_OFFSET_ADDR);
}
#endif

static void dma_init()
{
    unsigned int tmp_reg;
    g_host.dma_addr = (unsigned int*)g_dma_desc; /* Used to store the DMA descriptor chain, descriptors are word-aligned. */

    tmp_reg = readl(EMMC_BASE_ADDR + MCI_BMOD);
    tmp_reg |= BMOD_SWR | BURST_LENGTH | BURST_TYPE;
    writel(tmp_reg, EMMC_BASE_ADDR + MCI_BMOD);

    /* enable inner DMA mode and close intr of MMC host controler */
    tmp_reg = readl(EMMC_BASE_ADDR + MCI_CTRL);
    tmp_reg |= USE_INTERNAL_DMA | INTR_EN;
    writel(tmp_reg, EMMC_BASE_ADDR + MCI_CTRL);
}

static void mmc_haddr_extend_config(void)
{
    int ret = 0;
    unsigned int lpsc_addr_extend;

#define EMMC_ADDR_REMAP_MODE (1 << 16)
#define EMMC_HADDR_OFFSET    (8 << 12)

    lpsc_addr_extend = readl(LP_SUBSYS_BASE + LP_SUBSYS_CSR_LPSC_ADDR_EXTEND0_OFFSET_ADDR);
    lpsc_addr_extend |= (EMMC_ADDR_REMAP_MODE | EMMC_HADDR_OFFSET);
    writel(lpsc_addr_extend, LP_SUBSYS_BASE + LP_SUBSYS_CSR_LPSC_ADDR_EXTEND0_OFFSET_ADDR);
}

int emmc_init(void)
{
    unsigned int reg_val;
    unsigned int retry = 0;
    int ret = 0;

#if defined(CONFIG_HI3093_SPI_BOOT)
    mmc_set_io_cfg();
#endif
    /* soft reset MMC */
    reg_val = readl(LPSC_EMMC_CRG_CTRL_ADDR);
    reg_val |= 0x3;
    writel(reg_val, LPSC_EMMC_CRG_CTRL_ADDR);
    /* LP频率200M, 根据FS文档，延时2us足够，EDA版本采用2us*/
#ifndef EDA_SIMULATION
    udelay(10);
#else
    udelay(2);
#endif
    reg_val &= ~0x3;
    writel(reg_val, LPSC_EMMC_CRG_CTRL_ADDR);

    mmc_haddr_extend_config();

    /*EMMC phase cfg change to a fixed value, 180 degrees for sc_sdio_drv_ps_sel*/
    writel(0x8, LPSC_EMMC_FUNC_CTRL_ADDR);

    // clear MMC host intr
    writel(ALL_INT_CLR, EMMC_BASE_ADDR + MCI_RINTSTS);

    // set intr mask
    writel(DTO_INT_MASK, EMMC_BASE_ADDR + MCI_INTMASK);

    /* enable intr of MMC host controler */
    reg_val  = readl(EMMC_BASE_ADDR + MCI_CTRL);
    reg_val &= ~INTR_EN;
    reg_val |= INTR_EN;
    writel(reg_val, EMMC_BASE_ADDR + MCI_CTRL);

    // EMMC_CARDTHRCTL
    writel(EMMC_CARDTHRCTL_VALUE, EMMC_BASE_ADDR + EMMC_CARDTHRCTL_OFFSET_ADDR);

    /* set dma reg */
    reg_val  = readl(EMMC_BASE_ADDR + MCI_CTRL);
    reg_val |= CTRL_RESET | FIFO_RESET | DMA_RESET;
    writel(reg_val, EMMC_BASE_ADDR + MCI_CTRL);

    /* set timeout param */
    writel((DATA_TIMEOUT | RESPONSE_TIMEOUT), EMMC_BASE_ADDR + MCI_TIMEOUT);

    /* set FIFO param */
    reg_val  = 0;
    reg_val |= BURST_SIZE | RX_WMARK | TX_WMARK;
    writel(reg_val, EMMC_BASE_ADDR + MCI_FIFOTH);

    // hw_efuse_apply_patch(EFUSE_PATCH_ID_MMC_INIT);
    dma_init();
    /*
     * 分别尝试400KHZ,300KHZ,200KHZ，100KHZ的频率
     * 作为卡初始化频率,从高到低进行。如果全部频率都尝试失败
     * 则返回错误给上层
     */
    do
    {
        /*set emmc clock */
        ret = mmc_config_clock(f_clk[retry]);
        if (0 != ret)
        {
            printf("[M]:0\n");
            printf("%s:%d\n",__func__,__LINE__);
            return ret;
        }
        /*规范规定需要等待1ms加74周期*/
#ifndef EDA_SIMULATION
        mdelay(2);
#endif
        /*  init emmc card */
        ret = mmc_card_ready();
        if (0 == ret)
        {
            return 0;
        }
        retry++;
    }while(retry < MAX_CLK_COUNT);

    printf("[M]:1\n");
    printf("%s:%d\n",__func__,__LINE__);

    return ret;
}

/*****************************************************************************
 函 数 名  : mmc_non_dma_read
 功能描述  : EMMC读数据接口(FIFO方式)
 输入参数  : unsigned char *buf: 数据的存放地址
             unsigned int start: 读EMMC的起始block
             unsigned int num: 读数据的block数量
             unsigned int retry:出错尝试次数
 输出参数  : 无
 返 回 值  : int
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2018年10月23日
    修改内容   : 新生成函数

*****************************************************************************/
int mmc_non_dma_read(unsigned char *buf, unsigned int start, unsigned int num, unsigned int retry)
{
    int ret;
    do
    {
        ret = mmc_non_dma_rw_blcoks(start, EMMC_READ, num, buf);
        if(0 == ret)
        {
            return 0;
        }
        if(retry > 0)
            retry--;
    }while(retry);
    printf("[M]:2\n");

    printf("%s:%d\n",__func__,__LINE__);

    return ret;
}

/*****************************************************************************
 函 数 名  : mmc_non_dma_write
 功能描述  : EMMC写数据接口(FIFO方式)
 输入参数  : unsigned char *buf: 待写入的数据地址
             unsigned int start: 写EMMC的起始block
             unsigned int num: 写数据的block数量
             unsigned int retry:出错尝试次数
 输出参数  : 无
 返 回 值  : int
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2018年10月23日
    修改内容   : 新生成函数

*****************************************************************************/
int mmc_non_dma_write(unsigned char *buf, unsigned int start, unsigned int num, unsigned int retry)
{
    int ret;
    do
    {
        ret = mmc_non_dma_rw_blcoks(start, EMMC_WRITE, num, buf);
        if (0 == ret)
        {
            return 0;
        }
        if(retry > 0)
            retry--;
    }while(retry);
    printf("[M]:3\n");

    return ret;
}

/****************** DMA RW ******************************************/
static int mmc_irq_dma_read(dma_haddr_t buf, unsigned int start, unsigned int num)
{
    unsigned int group_read_num;
    unsigned int last_group_block_num;
    unsigned int dma_start;
    dma_haddr_t dma_buf;

    dma_start = start;
    dma_buf = buf;
    group_read_num = num / MAX_ONE_TIME_TRANSMIT_BLOCKS;
    printf("read %d groups of data\n", group_read_num);
    last_group_block_num = num % MAX_ONE_TIME_TRANSMIT_BLOCKS;

    for (int i = 0; i < group_read_num; i++) {
        printf("read group %d ...\n", i);
        if(mmc_irq_rw_blcoks(dma_start, MMC_DATA_READ, MAX_ONE_TIME_TRANSMIT_BLOCKS, dma_buf)) {
            return -EINVAL;
        }

        dma_start += MAX_ONE_TIME_TRANSMIT_BLOCKS;
        dma_buf += MAX_ONE_TIME_TRANSMIT_BLOCKS * MMC_BLK_SIZE;
    }

    if (last_group_block_num) {
        if(mmc_irq_rw_blcoks(dma_start, MMC_DATA_READ, last_group_block_num, dma_buf)) {
            return -EINVAL;
        }
    }

    return 0;
}

static int mmc_irq_dma_write(dma_haddr_t buf, unsigned int start, unsigned int num)
{
    unsigned int group_write_num;
    unsigned int last_group_block_num;
    unsigned int dma_start;
    dma_haddr_t dma_buf;

    dma_start = start;
    dma_buf= buf;
    group_write_num = num / MAX_ONE_TIME_TRANSMIT_BLOCKS;
    printf("write %d groups of data\n", group_write_num);
    last_group_block_num = num % MAX_ONE_TIME_TRANSMIT_BLOCKS;

    for (int i = 0; i < group_write_num; i++) {
        printf("write group %d ...\n", i);
        if(mmc_irq_rw_blcoks(dma_start, MMC_DATA_WRITE, MAX_ONE_TIME_TRANSMIT_BLOCKS, dma_buf)) {
            return -EINVAL;
        }

        dma_start += MAX_ONE_TIME_TRANSMIT_BLOCKS;
        dma_buf += MAX_ONE_TIME_TRANSMIT_BLOCKS * MMC_BLK_SIZE;
    }

    if (last_group_block_num) {
        if(mmc_irq_rw_blcoks(dma_start, MMC_DATA_WRITE, last_group_block_num, dma_buf)) {
            return -EINVAL;
        }
    }

    return 0;
}

int mmc_dma_read(unsigned int buf, unsigned int start, unsigned int num)
{
    int ret;
    dma_haddr_t dma_buf;

    if (!buf || (num == 0)) {
        printf("read param error!\n");
        return -EINVAL;
    }

    dma_buf = (dma_haddr_t)buf;
    ret = mmc_irq_dma_read(dma_buf, start, num);
    return ret;
}

int mmc_dma_write(unsigned char *buf, unsigned int start, unsigned int num)
{
    int ret;
    dma_haddr_t dma_buf;
    dma_haddr_t min_dma_addr;

    if (!buf || (num == 0)) {
        printf("write param error!\n");
        return -EINVAL;
    }

    dma_buf = (dma_haddr_t)buf;
    ret = mmc_irq_dma_write(dma_buf, start, num);
    return ret;
}

/*****************************************************************************
 函 数 名  : mmc_change_area
 功能描述  : EMMC切换分区(FIFO方式)
 输入参数  : unsigned int index: 分区标号,0:user,1-2:boot,3:RPMB,4-7:GPP
             unsigned char *buffer: 存放扩展寄存器的值
             unsigned int retry:出错尝试次数
 输出参数  : 无
 返 回 值  : int
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2018年10月23日
    修改内容   : 新生成函数

*****************************************************************************/
int mmc_non_dma_change_area(unsigned char index, unsigned int retry)
{
    int ret;
    unsigned char tmp_reg;
    unsigned char buffer[512] = {0};
    unsigned char index_mask = 7;/*bit[0-2]*/

    /*check parameter*/
    if(index > index_mask)
    {
        printf("%s:%d\n",__func__,__LINE__);
        return -1;
    }
    /*读取EXT_CSD寄存器*/
    ret = mmc_non_dma_get_ext_csd(buffer);
    if (0 != ret)
    {
        printf("%s:%d\n",__func__,__LINE__);
        return ret;
    }

    tmp_reg = buffer[EXT_CSD_PART_CONF];
    tmp_reg &= ~ PART_ACCESS_MASK;
    tmp_reg |= index;

    /*切换分区*/
    do
    {
        ret = emmc_switch(EXT_CSD_PART_CONF, tmp_reg);
        if (0 == ret)
        {
           return 0;
        }
        if(retry > 0)
            retry--;
    }while(retry);
    printf("[M]:4\n");
    printf("%s:%d\n",__func__,__LINE__);

    return ret;
}

