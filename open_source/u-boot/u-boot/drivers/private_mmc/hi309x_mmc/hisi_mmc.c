#include "hisi_mmc.h"
#include "emmc_addr_define.h"

unsigned int delay_count = 1;
unsigned int run_flk = F_MAX;
unsigned int card_caps = 0;
unsigned int card_csd[4] = {0};
#define  warn_on(count, ID)  if(count % 2000000 == 0) printf("TOUT:%d\n", ID)
/*****************************************************************************
 函 数 名  : mmc_wait_cmd
 功能描述  : 等待CMD寄存器的bit31自动清零
 输入参数  : 无
 输出参数  : 无
 返 回 值  : int
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2018年10月23日
    修改内容   : 新生成函数

*****************************************************************************/
static int mmc_wait_cmd(void)
{
    unsigned int wait_retry_count  = 0;
    unsigned int reg_data = 0;

    do
    {
        /*
           Check if CMD::start_cmd bit is clear.
           start_cmd = 0 means MMC Host controller has loaded registers
           and next command can be loaded in.
         */
        reg_data = readl(EMMC_BASE_ADDR + MCI_CMD);
        if ((reg_data & START_CMD) == 0)
        {
            return 0;
        }

        /* Check if Raw_Intr_Status::HLE bit is set. */
        reg_data = readl(EMMC_BASE_ADDR + MCI_RINTSTS);
        if (reg_data & HLE_INT_STATUS)
        {
            writel(reg_data, EMMC_BASE_ADDR + MCI_RINTSTS);
            printf("[HM]:0\n");
            return -1;
        }
#ifndef EDA_SIMULATION
        udelay(delay_count);
#endif
        /* Check if number of retries for this are over. */
        wait_retry_count++;
        warn_on(wait_retry_count, 0);
    }while (wait_retry_count < MAX_RETRY_COUNT);
    printf("[HM]:1");
    return -1;
}

/*****************************************************************************
 函 数 名  : mmc_exec_cmd
 功能描述  : 准备命令寄存器各个bit位参数，开始执行命令
 输入参数  : mmc_cmd *cmd: 操作参数
 输出参数  : 无
 返 回 值  : int
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2018年10月23日
    修改内容   : 新生成函数

*****************************************************************************/
static void mmc_wait_card_idle(void)
{
    unsigned int value;
    unsigned int retry = MMC_RETRY_WAIT_CARD_IDLE;

    do {
        value = readl(EMMC_BASE_ADDR + MCI_STATUS);
        if (!(value & DATA_BUSY)) {
            return;
        }

        udelay(1);
    } while (retry--);
    printf("mmc wait card idle is %d\n", retry);
}

static int mmc_exec_cmd(mmc_cmd *cmd)
{
    volatile cmd_arg_s cmd_regs;
    int ret = 0;

    cmd_regs.cmd_arg = 0;
    /* write command parameter to CMDARG register */
    writel(cmd->arg, EMMC_BASE_ADDR + MCI_CMDARG);

    cmd_regs.bits.use_hold_reg = 1;
    if (cmd->data_flag)
    {
        cmd_regs.bits.data_transfer_expected = 1;
        if (cmd->data_flag & (MMC_DATA_WRITE | MMC_DATA_READ))
        {
            cmd_regs.bits.transfer_mode = 0;
        }

        if (cmd->data_flag & MMC_DATA_WRITE)
        {
            cmd_regs.bits.read_write = 1;
        }
        else if (cmd->data_flag & MMC_DATA_READ)
        {
            cmd_regs.bits.read_write = 0;
        }
        else
        {
            return -1;
        }
    }
    else
    {
        cmd_regs.bits.data_transfer_expected = 0;
        cmd_regs.bits.transfer_mode = 0;
        cmd_regs.bits.read_write = 0;
    }

    cmd_regs.bits.stop_abort_cmd = 0;
    cmd_regs.bits.wait_prvdata_complete = 1;

    switch (mmc_resp_type(cmd->cmd_flag))
    {
    case MMC_RSP_NONE:
        cmd_regs.bits.response_expect = 0;
        cmd_regs.bits.response_length = 0;
        cmd_regs.bits.check_response_crc = 0;
        break;
    case MMC_RSP_R1:
    case MMC_RSP_R1B:
        cmd_regs.bits.response_expect = 1;
        cmd_regs.bits.response_length = 0;
        cmd_regs.bits.check_response_crc = 1;
        break;
    case MMC_RSP_R2:
        cmd_regs.bits.response_expect = 1;
        cmd_regs.bits.response_length = 1;
        cmd_regs.bits.check_response_crc = 1;
        break;
    case MMC_RSP_R3:
        cmd_regs.bits.response_expect = 1;
        cmd_regs.bits.response_length = 0;
        cmd_regs.bits.check_response_crc = 0;
        break;
    case MMC_RSP_R1_NO_CRC:
        cmd_regs.bits.response_expect = 1;
        cmd_regs.bits.response_length = 0;
        cmd_regs.bits.check_response_crc = 0;
        break;
    default:
        printf("[HM]:%02x-0x%x-0x%x\n", mmc_resp_type(cmd->cmd_flag),
                cmd->opcode, cmd->arg);
        return -1;
    }

    if (cmd->opcode == MMC_GO_IDLE_STATE)
    {
        cmd_regs.bits.send_initialization = 1;
    }
    else
    {
        cmd_regs.bits.send_initialization = 0;
    }

    cmd_regs.bits.card_number = 0; /* 保留位 */
    cmd_regs.bits.cmd_index = cmd->opcode;
    cmd_regs.bits.send_auto_stop = 0; /* 单块传输不需要发送STOP */
    cmd_regs.bits.start_cmd = 1;
    cmd_regs.bits.update_clk_reg_only = 0;

    if ((cmd->opcode == MMC_READ_MULTIPLE_BLOCK) || (cmd->opcode == MMC_WRITE_MULTIPLE_BLOCK)) {
        cmd_regs.bits.send_auto_stop = 1;
    }
    mmc_wait_card_idle();
    writel(cmd_regs.cmd_arg, EMMC_BASE_ADDR + MCI_CMD);

    ret = mmc_wait_cmd();
    if (0 != ret)
    {
        printf("[HM]:2\n");
        return ret;
    }

    return 0;
}

/*****************************************************************************
函 数 名  : mmc_cmd_done
功能描述  : 等待命令完成中断
输入参数  : unsigned int stats: 中断寄存器的值
            mmc_cmd *cmd: 操作参数
输出参数  : 无
返 回 值  : int
调用函数  :
被调函数  :

修改历史  :
1.日    期   : 2018年10月23日
修改内容   : 新生成函数

*****************************************************************************/
static int mmc_cmd_done(unsigned int stats, mmc_cmd *cmd)
{
    unsigned int i;
    unsigned long addr;

    for (i = 0; i < 4; i++)
    {
        addr = (unsigned long)EMMC_BASE_ADDR + MCI_RESP0 + i * 0x4;
        cmd->rsp[i] = readl(addr);
    }
    if (stats & RTO_INT_STATUS)
    {
        printf("[HM]:3-0x%x\n", stats);
        return -1;
    }
    else if (stats & (RCRC_INT_STATUS | RE_INT_STATUS))
    {
        // printf("[HM]:4-0x%x\n", stats);
        // return -1;
    }

    return 0;
}

/*****************************************************************************
 函 数 名  : mmc_wait_cmd_complete
 功能描述  : 等待命令完成中断
 输入参数  : mmc_cmd *cmd: 操作参数
 输出参数  : 无
 返 回 值  : int
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2018年10月23日
    修改内容   : 新生成函数

*****************************************************************************/
static int mmc_wait_cmd_complete(mmc_cmd *cmd)
{
    unsigned int cmd_retry_count = 0;
    unsigned int cmd_irq_reg = 0;
    int ret;

    do
    {
        cmd_irq_reg = readl(EMMC_BASE_ADDR + MCI_RINTSTS);

        if (cmd_irq_reg & CD_INT_STATUS)
        {
            writel((CD_INT_STATUS | RTO_INT_STATUS |
                    RCRC_INT_STATUS | RE_INT_STATUS),
                    EMMC_BASE_ADDR + MCI_RINTSTS);

            ret = mmc_cmd_done(cmd_irq_reg, cmd);

            return ret;
        }
        cmd_retry_count++;
#ifndef EDA_SIMULATION
        udelay(delay_count);
#endif
        warn_on(cmd_retry_count, 1);
    } while (cmd_retry_count < MAX_RETRY_COUNT);

    return -1;
}

/*****************************************************************************
 函 数 名  : mmc_wait_data_complete
 功能描述  : 等待数据传输完成
 输入参数  : 无
 输出参数  : 无
 返 回 值  : int
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2018年10月23日
    修改内容   : 新生成函数

*****************************************************************************/
static int mmc_wait_data_complete(mmc_cmd *cmd)
{
    unsigned int cmd_retry_count = 0;
    unsigned int data_irq_reg = 0;
    unsigned int retry_count = MAX_RETRY_COUNT;

    if (cmd->data_flag & MMC_DATA_WRITE) {
        do {
            if (readl(EMMC_BASE_ADDR + MCI_RINTSTS) & SBE_INT_STATUS) {
                break;
            }
            udelay(1);
            retry_count--;
        } while (retry_count);

        if (retry_count == 0) {
            printf("[HM]wait write timeout");
        }
    }

    do
    {
        data_irq_reg = readl(EMMC_BASE_ADDR + MCI_RINTSTS);
        if (data_irq_reg & DTO_INT_STATUS)
        {
            /*check data interrupt status*/
            if (data_irq_reg & (HTO_INT_STATUS | DRTO_INT_STATUS))
            {
                printf("[HM]:5-0x%x\n", data_irq_reg);
                return -1;
            }
            else if (data_irq_reg & (EBE_INT_STATUS | FRUN_INT_STATUS | DCRC_INT_STATUS))
            {
                printf("[HM]:6-0x%x-0x%x\n", data_irq_reg, readl(EMMC_BASE_ADDR + MCI_STATUS));
                return -1;
            }

            /* clear interrupt status */
            writel((DTO_INT_STATUS | HTO_INT_STATUS | DRTO_INT_STATUS |
                      EBE_INT_STATUS | SBE_INT_STATUS | FRUN_INT_STATUS |
                      DCRC_INT_STATUS), EMMC_BASE_ADDR + MCI_RINTSTS);

            return 0;
        }
        cmd_retry_count++;
#ifndef EDA_SIMULATION
        udelay(delay_count);
#endif
    } while (cmd_retry_count < MAX_RETRY_COUNT);

    data_irq_reg = readl(EMMC_BASE_ADDR + MCI_STATUS);
    printf("[HM]:7-0x%x\n", data_irq_reg);
    data_irq_reg = readl(EMMC_BASE_ADDR + MCI_RINTSTS);
    printf("[HM]:8-0x%x\n", data_irq_reg);

    return -1;
}

/*****************************************************************************
 函 数 名  : mmc_non_dma_request
 功能描述  : FIFO 方式请求控制器与卡之间的交互
 输入参数  : mmc_cmd *cmd: 操作参数
             unsigned char *buf: EMMC读写buffer
 输出参数  : 无
 返 回 值  : int
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2018年10月23日
    修改内容   : 新生成函数

*****************************************************************************/
static int mmc_non_dma_request(mmc_cmd *cmd,unsigned char *buf)
{
    unsigned int i;
    int ret = 0;
    unsigned int tmp_reg;
    unsigned int fifo_count = 0;
    unsigned int card_count = 0;
    unsigned int len;

    if(buf != NULL)
    {
        /* write block size to MCI_BLKSIZ(0x863001C), and data size to MCI_BYTCNT(0x8630020) */
        len = cmd->n_block * BLOCK_SIZE;
        writel(len,EMMC_BASE_ADDR + MCI_BYTCNT);
        writel(BLOCK_SIZE,EMMC_BASE_ADDR + MCI_BLKSIZ);

        /* using slave transfer data and reset fifo, waiting fifo_reset clear */
        tmp_reg = readl(EMMC_BASE_ADDR + MCI_CTRL);
        tmp_reg |= FIFO_RESET;
        tmp_reg &= USE_INTERNAL_SLAVE;
        writel(tmp_reg, EMMC_BASE_ADDR + MCI_CTRL);

        do
        {
            tmp_reg = readl(EMMC_BASE_ADDR + MCI_CTRL);
            fifo_count++;
#ifndef EDA_SIMULATION
            udelay(delay_count);
#endif
            warn_on(fifo_count, 3);
            if (fifo_count >= MAX_RETRY_COUNT)
            {
                printf("[HM]:9\n");
                return -1;
            }
        } while (tmp_reg & FIFO_RESET);

        /* write data form buffer to fifo if it's write operation */
        if (cmd->data_flag & MMC_DATA_WRITE)
        {
            for (i = 0; i < len; i += sizeof(unsigned int))
            {
                writel(*((unsigned int *)(buf + i)), EMMC_BASE_ADDR + EMMC_DATA_OFFSET_ADDR);
            }
        }
    }
    else
    {
        writel(0, EMMC_BASE_ADDR + MCI_BYTCNT);
        writel(0, EMMC_BASE_ADDR + MCI_BLKSIZ);
    }

    /* config cmd and cmdarg register ,and execute conmand */
    ret = mmc_exec_cmd(cmd);
    if(0 != ret)
    {
        return ret;
    }

    /* check for command done interrupts, waiting for command completion */
    ret = mmc_wait_cmd_complete(cmd);
    if(0 != ret)
    {
        return ret;
    }

    if(buf != NULL)
    {
        /* check for data transfer over INT, waiting for data completion */
        ret = mmc_wait_data_complete(cmd);
        if(0 != ret)
        {
            return ret;
        }

        /* read data to buffer if it's a read operation */
        if(cmd->data_flag & MMC_DATA_READ)
        {
            for(i = 0;i < BLOCK_SIZE; i += sizeof(unsigned int))
            {
               *((unsigned int *)(buf + i)) = readl(EMMC_BASE_ADDR + EMMC_DATA_OFFSET_ADDR);
            }
        }

        /* wait card idle if it's a write operation */
        if (cmd->data_flag & MMC_DATA_WRITE)
        {
            do{
                tmp_reg = readl(EMMC_BASE_ADDR + MCI_STATUS);
                if(!(tmp_reg & DATA_BUSY))
                {
                    writel(ALL_INT_CLR,EMMC_BASE_ADDR + MCI_RINTSTS);
                    return 0;
                }

                card_count++;
#ifndef EDA_SIMULATION
                udelay(delay_count);
#endif
                warn_on(card_count, 4);
            }while(card_count < MAX_RETRY_COUNT);
            printf("[HM]:10\n");
            return -1;

        }
    }

    /* clear all INT */
    writel(ALL_INT_CLR,EMMC_BASE_ADDR + MCI_RINTSTS);
    return 0;
}

/*****************************************************************************
 函 数 名  : mmc_non_dma_rw_blcoks
 功能描述  : EMMC读写接口(DMA方式)
 输入参数  : unsigned int start_addr: 读/写的起始地址
             unsigned int type: 0:读，1: 写
             unsigned int num: 读写数据block大小
             char *buf: 外部buffer，存放读写的内容
 输出参数  : 无
 返 回 值  : int
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2018年10月23日
    修改内容   : 新生成函数

*****************************************************************************/
int mmc_non_dma_rw_blcoks(unsigned int start_addr, unsigned int type, unsigned int num, unsigned char *buf)
{
    int ret = 0;
    mmc_cmd cmd = {0};
    unsigned int block_addr = start_addr;

    cmd.data_flag = (EMMC_READ == type) ? MMC_DATA_READ : MMC_DATA_WRITE;
    cmd.cmd_flag = MMC_RSP_R1;

    while (num)
    {
        cmd.n_block = 1;
        cmd.arg = block_addr;
        cmd.opcode = (EMMC_READ == type) ? MMC_READ_SINGLE_BLOCK : MMC_WRITE_BLOCK;

        ret = mmc_non_dma_request(&cmd, buf);
        if (0 != ret)
        {
           printf("[HM]:11\n");
           writel(ALL_INT_CLR, EMMC_BASE_ADDR + MCI_RINTSTS);
           return ret;
        }

        num--;
        block_addr++;
        buf = buf + BLOCK_SIZE;
    }

    return 0;
}

/********************** mmc dma write ******************************************************/
static unsigned int mmc_get_relative_addr(dma_haddr_t phy_addr)
{
    dma_haddr_t dmac_haddr = 0;
    dma_haddr_t relative_addr;
    u8 offset;
    unsigned int lp_haddr_extend;
    unsigned int lpsc_addr_sel;
    unsigned int remap_mode;

    lp_haddr_extend = readl(LP_SUBSYS_BASE + LP_SUBSYS_CSR_LPSC_ADDR_EXTEND0_OFFSET_ADDR);
    lpsc_addr_sel = readl(LP_SUBSYS_BASE + LP_SUBSYS_CSR_LPSC_ADDR_SEL_OFFSET_ADDR);

    /* emmc作为master的remap模式偏移[16, 17] , 0x3表示2bit */
    remap_mode = ((lp_haddr_extend >> 16) & 0x3);
    switch (remap_mode) {
        case 0x1:
            offset = ((lp_haddr_extend >> 12) & 0xf); /* emmc作为master的输出地址偏移[12,15] 0xf表示4bit */
            dmac_haddr = ((dma_haddr_t)offset << 28); /* 模式1下地址偏移的最小偏移为 1 << 28 */
            break;

        case 0x2:
            offset = ((lpsc_addr_sel >> 8) & 0x3);    /* emmc访问时，高位地址选择偏移[8,9], 0x3表示2bit */
            dmac_haddr = ((dma_haddr_t)offset << 32); /* 高位地址为[33,32]位 */
            break;

        default:
            break;
    }

    if (phy_addr < dmac_haddr) {
        printf("[HM]lp(0x%x) addr(0x%llx) error\n", lp_haddr_extend, phy_addr);
        return phy_addr;
    }

    relative_addr = phy_addr - dmac_haddr;

    return (unsigned int)relative_addr;
}

static int mmc_setup_data(dma_haddr_t buf, unsigned int len)
{
#define ONE_DES_SIZE_SHIFT 12
    unsigned int des_cnt;
    unsigned int next_addr;
    unsigned int tmp;
    struct emmc_des *des;
    unsigned int base_next_addr;
    unsigned int base_buf_addr;
    unsigned int times, left_size;

    if (!len) {
        printf("[HM](HLE)setup data failed, len can not be 0!\n");
        return -EINVAL;
    }

    /* dma_addr指针类型为(unsigned int*), 4byte步长增加 */
    tmp = (unsigned int)(g_host.dma_addr + sizeof(struct emmc_des) / 4);
    base_next_addr = mmc_get_relative_addr((dma_haddr_t)tmp);
    base_buf_addr = mmc_get_relative_addr((dma_haddr_t)buf);
    des = (struct emmc_des *)(g_host.dma_addr);

    times = len / MAX_ONE_DES_SIZE;
    left_size = len % MAX_ONE_DES_SIZE;

    for (des_cnt = 0; des_cnt < times; des_cnt++) {
        des[des_cnt].idmac_des_ctrl = DMA_DES_OWN | DMA_DES_NEXT_DES;
        des[des_cnt].idmac_des_buf_size = MAX_ONE_DES_SIZE;
        des[des_cnt].idmac_des_buf_addr = base_buf_addr + (des_cnt << ONE_DES_SIZE_SHIFT);
        next_addr = base_next_addr + des_cnt * sizeof(struct emmc_des);
        des[des_cnt].idmac_des_next_addr = next_addr;
    }

    if (left_size) {
        des[des_cnt].idmac_des_ctrl = DMA_DES_OWN | DMA_DES_NEXT_DES | DMA_DES_LAST_DES;
        des[des_cnt].idmac_des_buf_size = left_size;
        des[des_cnt].idmac_des_buf_addr = base_buf_addr + (des_cnt << ONE_DES_SIZE_SHIFT);
        des[des_cnt].idmac_des_next_addr = 0;
    } else {
        des[des_cnt - 1].idmac_des_ctrl |= DMA_DES_LAST_DES;
        des[des_cnt - 1].idmac_des_next_addr = 0;
    }

    des[0].idmac_des_ctrl |= DMA_DES_FIRST_DES;

    return 0;
}


static int mmc_ctrl_reset(unsigned int reset)
{
    unsigned int ctrl;
    unsigned int count = 500; /* 500ms */

    ctrl = readl(EMMC_BASE_ADDR + MCI_CTRL);
    ctrl |= reset;
    writel(ctrl, EMMC_BASE_ADDR + MCI_CTRL);

    /* wait till resets clear */
    do {
        ctrl = readl(EMMC_BASE_ADDR + MCI_CTRL);
        if (!(ctrl & reset)) {
            return 0;
        }
        mdelay(1);
        count--;
    } while (count);

    printf("Timeout resetting block (ctrl reset 0x%x)\n", ctrl & reset);

    return -ETIMEDOUT;
}

void mmc_idma_stop(void)
{
    unsigned int tmp_reg;

    /* Disable and reset the IDMAC interface */
    tmp_reg = readl(EMMC_BASE_ADDR + MCI_CTRL);
    tmp_reg &= ~HISI_MMC_CTRL_USE_IDMAC;
    tmp_reg |= HISI_MMC_CTRL_DMA_RESET;
    writel(tmp_reg, EMMC_BASE_ADDR + MCI_CTRL);

    tmp_reg = readl(EMMC_BASE_ADDR + MCI_BMOD);
    tmp_reg &= ~BMOD_DMA_EN;
    writel(tmp_reg, EMMC_BASE_ADDR + MCI_BMOD);
}

static void mmc_idma_start(void)
{
    unsigned int tmp_reg;
    unsigned int relative_addr;
    int ret;

    /* Make sure to reset DMA in case we did PIO before this */
    ret = mmc_ctrl_reset(HISI_MMC_CTRL_DMA_RESET);
    if (ret) {
        return;
    }

    /* Software reset of DMA */
    tmp_reg = readl(EMMC_BASE_ADDR + MCI_BMOD);
    tmp_reg |= HISI_MMC_IDMAC_SWRESET;
    writel(tmp_reg, EMMC_BASE_ADDR + MCI_BMOD);
    mdelay(1);

    relative_addr = mmc_get_relative_addr((dma_haddr_t)(g_host.dma_addr));
    writel(relative_addr, EMMC_BASE_ADDR + MCI_DBADDR);

    /* Select IDMAC interface */
    tmp_reg = readl(EMMC_BASE_ADDR + MCI_CTRL);
    tmp_reg |= USE_INTERNAL_DMA;
    writel(tmp_reg, EMMC_BASE_ADDR + MCI_CTRL);

    tmp_reg = readl(EMMC_BASE_ADDR + MCI_BMOD);
    tmp_reg |= BMOD_DMA_EN;
    writel(tmp_reg, EMMC_BASE_ADDR + MCI_BMOD);

    return;
}

static int mmc_irq(mmc_cmd *cmd)
{
    unsigned int irq_mask;
    int ret;

    /* get irq mask value and mask all irq */
    irq_mask = readl(EMMC_BASE_ADDR + MCI_INTMASK);
    writel(0x0, EMMC_BASE_ADDR + MCI_INTMASK);

    ret = mmc_wait_data_complete(cmd);
    mmc_idma_stop();

    /* 打开中断掩码 */
    writel(irq_mask, EMMC_BASE_ADDR + MCI_INTMASK);

    return ret;
}

static int mmc_dma_request(mmc_cmd *cmd, dma_haddr_t buf)
{
    int ret;
    unsigned int len;
    unsigned int tmp_reg;
    unsigned int fifo_count = WAIT_FIFO_RESET_RETRY_COUNT;
    unsigned int count = SYNC_DMA_RW_RETRY_COUNT * 2;

    writel(ALL_INT_CLR, EMMC_BASE_ADDR + MCI_RINTSTS);

    if (buf != 0) {
        /* Writing the block size and total number of bytes to be transferred in this request */
        len = cmd->n_block * BLOCK_SIZE;
        writel(len, EMMC_BASE_ADDR + MCI_BYTCNT);
        writel(BLOCK_SIZE, EMMC_BASE_ADDR + MCI_BLKSIZ);

        if (mmc_setup_data(buf, len)) {
            printf("[HM]mmc_setup_data error\n");
            return -EFAULT;
        }

        /* using dma transfer data and reset fifo, waiting fifo_reset clear */
        tmp_reg = readl(EMMC_BASE_ADDR + MCI_CTRL);
        tmp_reg |= (FIFO_RESET | USE_INTERNAL_DMA);
        writel(tmp_reg, EMMC_BASE_ADDR + MCI_CTRL);

        do {
            tmp_reg = readl(EMMC_BASE_ADDR + MCI_CTRL);
            fifo_count--;
            udelay(1);
            if (fifo_count == 0) {
                printf("fifo reset is timeout\n");
                return -EFAULT;
            }
        }while (tmp_reg & FIFO_RESET);

        mmc_idma_start();
    } else {
        writel(0, EMMC_BASE_ADDR + MCI_BYTCNT);
        writel(0, EMMC_BASE_ADDR + MCI_BLKSIZ);
    }

    /* config cmd and cmdarg register ,and execute conmand */
    if (mmc_exec_cmd(cmd)) {
        goto EXIT0;
    }

    /* check for command done interrupts, waiting for command completion */
    if (mmc_wait_cmd_complete(cmd)) {
        goto EXIT0;
    }

    /* wait irq and data trans complete */
    if (mmc_irq(cmd)) {
        goto EXIT0;
    }

    return 0;
EXIT0:
    mmc_idma_stop();
    return -EFAULT;
}

int mmc_irq_rw_blcoks(unsigned int start_addr, unsigned int type, unsigned int num, dma_haddr_t buf)
{
    int ret;
    mmc_cmd cmd = { 0 };
    cmd.arg = start_addr;
    cmd.n_block = num;
    cmd.data_flag = type;
    cmd.opcode = (type == MMC_DATA_READ) ? MMC_READ_MULTIPLE_BLOCK : MMC_WRITE_MULTIPLE_BLOCK;
    cmd.cmd_flag = MMC_RSP_R1;

    ret = mmc_dma_request(&cmd, buf);
    if (ret) {
        printf("[HM]:11, dma request failed\n");
        return ret;
    }

    return 0;
}

/*****************************************************************************
 函 数 名  : mmc_non_dma_get_ext_csd
 功能描述  : 读取扩展寄存器的值(FIFO方式)
 输入参数  : unsigned char *buf: 存放扩展寄存器的值
 输出参数  : 无
 返 回 值  : int
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2018年10月23日
    修改内容   : 新生成函数

*****************************************************************************/
int mmc_non_dma_get_ext_csd(unsigned char *buf)
{
    mmc_cmd cmd = {0};
    unsigned int ret;


    cmd.opcode = MMC_SEND_EXT_CSD;
    cmd.cmd_flag = MMC_RSP_R1;
    cmd.arg = 0;
    cmd.data_flag = MMC_DATA_READ;
    cmd.n_block = 1;

    ret = mmc_non_dma_request(&cmd, buf);
    if (0 != ret)
    {
       printf("[HM]:12\n");
       return ret;
    }

    return 0;

}

/*****************************************************************************
 函 数 名  : mmc_go_idle
 功能描述  : 发送CMD0，使卡进入idle状态
 输入参数  : 无
 输出参数  : 无
 返 回 值  : int
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2018年10月23日
    修改内容   : 新生成函数

*****************************************************************************/
static int mmc_go_idle(void)
{
    mmc_cmd cmd = {0};
    unsigned int ret;

    cmd.opcode = MMC_GO_IDLE_STATE;
    cmd.cmd_flag = MMC_RSP_NONE;
    cmd.arg = 0;
    cmd.data_flag = 0;

    ret = mmc_non_dma_request(&cmd, NULL);
    if(0 != ret)
    {
        printf("[HM]:13\n");
        return ret;
    }

#ifndef EDA_SIMULATION
    udelay(2000);
#else /*EDA_SIMULATION*/
    udelay(1000);
#endif

    return 0;
}

/*****************************************************************************
 函 数 名  : emmc_switch
 功能描述  : 发送CMD6设置扩展寄存器，CMD6之后设备可能显示busy,等待设备退出busy
 输入参数  : unsigned char index: 索引
             unsigned char value:值
             int timeout:超时设置
 输出参数  : 无
 返 回 值  : int
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2018年10月23日
    修改内容   : 新生成函数

*****************************************************************************/
int emmc_switch(unsigned char index, unsigned char value)
{
    mmc_cmd cmd = {0};
    unsigned int ret;
    unsigned int retries = 5000;

    memset(&cmd, 0, sizeof(mmc_cmd));

    /* [31:26]: 0, [25:24]: access, [23:16]: index, [15:8]: value, [7:3]: 0, [2:0]:command */
    cmd.opcode = MMC_SWITCH;
    cmd.cmd_flag = MMC_RSP_R1B;
    cmd.arg = (MMC_SWITCH_MODE_WRITE_BYTE << 24) | (index << 16) | (value << 8);

    ret = mmc_non_dma_request(&cmd, NULL);
    if (0 != ret)
    {
        return ret;
    }

    memset(&cmd, 0, sizeof(mmc_cmd));

    /* query device status */
    cmd.opcode = MMC_SEND_STATUS;
    cmd.cmd_flag = MMC_RSP_R1;
    cmd.arg = EMMC_RCA << 16;
    do
    {
        ret = mmc_non_dma_request(&cmd, NULL);

        if (0 != ret)
        {
            return ret;
        }
        else
        {
            if ((cmd.rsp[0] & MMC_STATUS_RDY_FOR_DATA) &&
                (cmd.rsp[0] & MMC_STATUS_CURR_STATE) != MMC_STATE_PRG)
            {
                break;
            }
            else if(cmd.rsp[0] & MMC_STATUS_MASK)
            {
                printf("[HM]:14-0x%x\n", cmd.rsp[0]);
                return -1;
            }
        }
#ifndef EDA_SIMULATION
        mdelay(delay_count);
#endif
    }while (retries--);

    return 0;
}

/*****************************************************************************
 函 数 名  : mmc_set_cclk
 功能描述  : 设置时钟分频系数
 输入参数  : unsigned int cclk: 目标时钟频率，当无法整除时，分频系数加1
 输出参数  : 无
 返 回 值  : int
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2018年10月23日
    修改内容   : 新生成函数

*****************************************************************************/
static int mmc_set_cclk(unsigned int cclk)
{
    int ret;
    unsigned int reg_value = 0;
    cmd_arg_s clk_cmd;

    clk_cmd.cmd_arg = 0;
    /*set card clk divider value, clk_divider = Fmmcclk/(Fmmc_cclk * 2) */
    if (cclk < MMC_CLK)
    {
        /*lint -save -e414*/
        reg_value = MMC_CLK / (cclk * 2);
        if (MMC_CLK % (cclk * 2))
        {
            reg_value++;
        }
        /*lint -restore*/
        if (reg_value > 0xFF)
        {
            reg_value = 0xFF;
        }
    }
    writel(reg_value, EMMC_BASE_ADDR + MCI_CLKDIV);

    /* 写命令寄存器CMD，使分频系数更新生效 */
    clk_cmd.cmd_arg = readl(EMMC_BASE_ADDR + MCI_CMD);
    clk_cmd.bits.start_cmd = 1;
    clk_cmd.bits.update_clk_reg_only = 1;
    writel(clk_cmd.cmd_arg, EMMC_BASE_ADDR + MCI_CMD);

    /* 等待CMD[31]自动清零 */
    ret = mmc_wait_cmd();
    if (ret != 0)
    {
        printf("[HM]:15\n");

        return ret;
    }

    return 0;
}

/*****************************************************************************
 函 数 名  : mmc_control_cclk
 功能描述  : 开关时钟
 输入参数  : unsigned int flag: 时钟的目的状态，ENABLE: 开，DISABLE: 关
 输出参数  : 无
 返 回 值  : int
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2018年10月23日
    修改内容   : 新生成函数

*****************************************************************************/
static int mmc_control_cclk(unsigned int flag)
{
    int ret;
    unsigned int regs = 0;
    cmd_arg_s cmd_reg;

    regs = readl(EMMC_BASE_ADDR + MCI_CLKENA);
    if (flag == ENABLE)
    {
        regs |= CCLK_ENABLE;
    }
    else
    {
        regs &= CCLK_DISABLE;
    }

    writel(regs, EMMC_BASE_ADDR + MCI_CLKENA);

    cmd_reg.cmd_arg = readl(EMMC_BASE_ADDR + MCI_CMD);
    cmd_reg.bits.start_cmd = 1;
    cmd_reg.bits.update_clk_reg_only = 1;
    writel(cmd_reg.cmd_arg, EMMC_BASE_ADDR + MCI_CMD);

    ret = mmc_wait_cmd();
    if (ret != 0)
    {
        printf("[HM]:16\n");
        return ret;
    }

    return 0;
}

/*****************************************************************************
 函 数 名  : mmc_config_clock
 功能描述  : 配置控制与卡的交互时钟
 输入参数  : unsigned int cclk: 目标时钟频率
 输出参数  : 无
 返 回 值  : int
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2018年10月23日
    修改内容   : 新生成函数

*****************************************************************************/
int mmc_config_clock(unsigned int cclk)
{
    int ret;

    /* 关时钟 */
    ret = mmc_control_cclk(DISABLE);
    if (0 != ret)
    {
        return ret;
    }

    /* 设置分频系数 */
    ret = mmc_set_cclk(cclk);
    if (0 != ret)
    {
        return ret;
    }

    /* 开时钟 */
    ret = mmc_control_cclk(ENABLE);
    if (0 != ret)
    {
        return ret;
    }
    return 0;
}

/*****************************************************************************
 函 数 名  : mmc_send_op_cond_iter
 功能描述  : 获取OCR寄存器的值，然后主机发送支持的OCR值
 输入参数  : ocr: 命令的ocr参数
 			rocr: 卡侧返回的ocr值
 输出参数  : 无
 返 回 值  : int
 调用函数  :
 被调函数  :
 说    明  : OCR寄存器寄存着设备的VDD电压概况和访问模式指示

 修改历史      :
  1.日    期   : 2018年10月23日
    修改内容   : 新生成函数

*****************************************************************************/
static int mmc_send_op_cond_iter(unsigned int ocr,unsigned int *rocr)
{
    unsigned int ret;
    mmc_cmd cmd = {0};

    cmd.opcode = MMC_SEND_OP_COND;
    cmd.data_flag = 0;
    cmd.cmd_flag = MMC_RSP_R3;
    cmd.arg = ocr;

    ret = mmc_non_dma_request(&cmd,NULL);
    if (0 != ret)
    {
        printf("[HM]:17\n");
        return -1;
    }
    if(rocr)
    {
        *rocr = cmd.rsp[0];
    }
    return 0;
}
/*****************************************************************************
 函 数 名  : mmc_select_voltage
 功能描述  : 协商电压值和访问方式
 输入参数  : ocr emmc卡返回的ocr值
 输出参数  : ocr 协商过后的ocr值
 返 回 值  : unsigned int
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2018年10月23日
    修改内容   : 新生成函数

*****************************************************************************/
unsigned int mmc_select_voltage(unsigned int ocr)
{
    unsigned int voltages = MMC_VDD_32_33 | MMC_VDD_33_34;

    /*select voltages*/
    ocr &= voltages;
    /*select access mode*/
    ocr = ocr | 1 << 30;

    return ocr;
}
/*****************************************************************************
 函 数 名  : mmc_get_cid
 功能描述  : 请求设备发送其唯一的设备标识号CID
 输入参数  : 无
 输出参数  : 无
 返 回 值  : int
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2018年10月23日
    修改内容   : 新生成函数

*****************************************************************************/
static int mmc_get_cid(void)
{
    mmc_cmd cmd = {0};
    unsigned int ret;

    cmd.opcode = MMC_ALL_SEND_CID;
    cmd.cmd_flag = MMC_RSP_R2;
    cmd.arg = 0;

    ret = mmc_non_dma_request(&cmd, NULL);
    if (0 != ret)
    {
        printf("[HM]:18\n");
        return ret;
    }

    return 0;
}

/*****************************************************************************
 函 数 名  : mmc_send_rca
 功能描述  : 主机发送CMD3,给设备分配一个相对设备地址(RCA),要求: RCA > 1
 输入参数  : 无
 输出参数  : 无
 返 回 值  : int
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2018年10月23日
    修改内容   : 新生成函数

*****************************************************************************/
static int mmc_send_rca(void)
{
    mmc_cmd cmd = {0};
    unsigned int rca = EMMC_RCA;
    unsigned int ret;

    /* [31-16]:rca,[15-0]:位填充 */
    cmd.opcode = MMC_SET_RELATIVE_ADDR;
    cmd.cmd_flag = MMC_RSP_R1;
    cmd.arg = rca << 16;

    ret = mmc_non_dma_request(&cmd,NULL);
    if (0 != ret)
    {
        printf("[HM]:19\n");
        return ret;
    }

    return 0;
}

/*****************************************************************************
 函 数 名  : mmc_select_card
 功能描述  : 主机发送带RCA的参数命令来选定设备，把设备至于transfer状态
 输入参数  : 无
 输出参数  : 无
 返 回 值  : int
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2018年10月23日
    修改内容   : 新生成函数

*****************************************************************************/
static int mmc_select_card(void)
{
    mmc_cmd cmd = {0};
    unsigned int rca = EMMC_RCA;
    unsigned int ret;

    /* [31-16]:rca,[15-0]:位填充 */
    cmd.opcode = MMC_SELECT_CARD;
    cmd.cmd_flag = MMC_RSP_R1;
    cmd.arg = rca << 16;;

    ret = mmc_non_dma_request(&cmd, NULL);
    if (0 != ret)
    {
        printf("[HM]:20\n");
        return ret;
    }

    return 0;
}

/*****************************************************************************
 函 数 名  : mmc_get_csd
 功能描述  : 主机发送CMD9获取设备的专用数据(CSD寄存器)，如块长度等
 输入参数  : 无
 输出参数  : 无
 返 回 值  : int
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2018年10月23日
    修改内容   : 新生成函数

*****************************************************************************/
int mmc_get_csd(void)
{
    mmc_cmd cmd = {0};
    unsigned int ret;


    /* [31-16]:rca,[15-0]:位填充 */
    cmd.opcode = MMC_SEND_CSD;
    cmd.cmd_flag = MMC_RSP_R2;
    cmd.arg = EMMC_RCA << 16;

    ret = mmc_non_dma_request(&cmd,NULL);
    if (0 != ret)
    {
        printf("[HM]:21\n");
        return ret;
    }

    memcpy(card_csd, cmd.rsp, 16);

    return 0;
}


/*****************************************************************************
 函 数 名  : mmc_set_bus_width
 功能描述  : 设置EMMC 卡总线位宽
 输入参数  : int width: 目标位宽
 输出参数  : 无
 返 回 值  : int
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2018年10月23日
    修改内容   : 新生成函数

*****************************************************************************/
static int mmc_set_bus_width(int width)
{
    int ret = 0;
    int reg_val = 0;

    ret = emmc_switch( EXT_CSD_BUS_WIDTH, EXT_CSD_BUS_WIDTH_8);
    if (0 != ret)
    {
        printf("[HM]:22\n");
        return ret;
    }

    /* set bus_width, clear bus width to 1bit first */
    reg_val = readl(EMMC_BASE_ADDR + MCI_CTYPE);

    reg_val &= ~(CARD_WIDTH_8 | CARD_WIDTH_4);

    if (MMC_BUS_WIDTH_8 == width)
    {
        reg_val |= CARD_WIDTH_8;
        writel(reg_val, EMMC_BASE_ADDR + MCI_CTYPE);
    }
    else if (MMC_BUS_WIDTH_4 == width)
    {
        reg_val |= CARD_WIDTH_4;
        writel(reg_val, EMMC_BASE_ADDR + MCI_CTYPE);
    }
    else
    {
        writel(reg_val, EMMC_BASE_ADDR + MCI_CTYPE);
    }

    return 0;
}

/*****************************************************************************
 函 数 名  : mmc_card_ready
 功能描述  : 初始化EMMC卡
 输入参数  : 无
 输出参数  : 无
 返 回 值  : int
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2018年10月23日
    修改内容   : 新生成函数

*****************************************************************************/

static int mmc_set_hs_timing(unsigned int timing, unsigned int *ctr_config_timing)
{
    int ret;
    unsigned int reg_val;
    u8 set_timing;

#define CONFIG_EXT_CSD_TIMING_HS_SPEED 0x1
#define CONFIG_EXT_CSD_TIMING_HS200    0x2
#define CONFIG_EXT_CSD_TIMING_HS400    0x3

    reg_val = readl(EMMC_BASE_ADDR + MCI_UHS_REG);

    if (timing == MMC_HS_SDR_MODE) {
        set_timing = CONFIG_EXT_CSD_TIMING_HS_SPEED;
        reg_val &= (~DDR_MODE_ENABLE);
    } else if (timing == MMC_HS_DDR_MODE) {
        set_timing = CONFIG_EXT_CSD_TIMING_HS_SPEED;
        reg_val |= DDR_MODE_ENABLE;
    } else if (timing == MMC_HS200_SDR_MODE) {
        set_timing = CONFIG_EXT_CSD_TIMING_HS200;
        reg_val &= (~DDR_MODE_ENABLE);
    } else if (timing == MMC_HS400_DDR_MODE) {
        set_timing = CONFIG_EXT_CSD_TIMING_HS400;
        reg_val |= DDR_MODE_ENABLE;
    } else {
        set_timing = 0x0;
        reg_val &= (~DDR_MODE_ENABLE);
    }

    ret = emmc_switch(EXT_CSD_HS_TIMING, set_timing);
    if (ret) {
        printf("[HM] set hs timing(0x%x) fail\n", set_timing);
        return ret;
    }

    /* 获取控制器应该配置的timing */
    *ctr_config_timing = reg_val;

    return 0;
}

int mmc_card_ready(void)
{
    unsigned int ret;
    unsigned int ocr = 0;
    unsigned int rocr = 0;
    unsigned int retry = CARD_RETRY_COUNT;
    unsigned int ctr_timing = 0;

    /* send CMD0,to put card into idle status */
    ret = mmc_go_idle();
    if (0 != ret)
    {
        printf("%s:%d\n",__func__,__LINE__);
        return ret;
    }

    /* send CMD1 to get and verify voltage type and range, and access mode.
       if CMD1's response is busy, we must wait for busy bit clear.
    */
    ret = mmc_send_op_cond_iter(0,&ocr);
    if (0 != ret)
    {
        printf("[HM]:23\n");
        printf("%s:%d\n",__func__,__LINE__);
        return ret;
    }

    ocr = mmc_select_voltage(ocr);
    do
    {
        ret = mmc_send_op_cond_iter(ocr,&rocr);
        if(ret != 0)
        {
            printf("[HM]:24\n");
            return ret;
        }

        retry--;
        if (0 == retry)
        {
            printf("[HM]:25\n");
            printf("%s:%d\n",__func__,__LINE__);
            return -1;
        }
#ifndef EDA_SIMULATION
        mdelay(10);
#endif

    }while (!(rocr & OCR_BUSY));/*这里退出条件为rocr中BIT31置1*/

    /* send CMD2 getting cid */
    ret = mmc_get_cid();
    if (0 != ret)
    {
        printf("%s:%d\n",__func__,__LINE__);
        return ret;
    }

    /* send CMD3 alloc relative device address */
    ret = mmc_send_rca();
    if (0 != ret)
    {
        printf("%s:%d\n",__func__,__LINE__);
        return ret;
    }

    /* send CMD9 getting card's csd info */
    ret  = mmc_get_csd();
    if (0 != ret)
    {
        printf("%s:%d\n",__func__,__LINE__);
        return ret;
    }

    /* send CMD7 to select a card */
    ret = mmc_select_card();
    if (0 != ret)
    {
        printf("%s:%d\n",__func__,__LINE__);
        return ret;
    }

    /* set 8bit width */
    ret = mmc_set_bus_width(MMC_BUS_WIDTH_8);
    if(0 != ret)
    {
        printf("%s:%d\n",__func__,__LINE__);
        return ret;
    }

    // rate mode
    ret = mmc_set_hs_timing(MMC_HS200_SDR_MODE, &ctr_timing);
    if (ret) {
        return ret;
    }
    writel(ctr_timing, EMMC_BASE_ADDR + MCI_UHS_REG);

    /* set clock frequency */
    ret = mmc_config_clock(run_flk);
    if(0 != ret)
    {
        printf("%s:%d\n",__func__,__LINE__);
        return ret;
    }

    return 0;
}
