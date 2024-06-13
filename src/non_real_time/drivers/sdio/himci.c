// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * hisilicon MMC Host driver
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#include "himci.h"
#include "lp_subsys_csr_addr_define.h"
#include "lp_subsys_c_union_define.h"
#include "lp_subsys.h"
#include "comm_knl_api.h"
#include "mmc_usr_api.h"
#ifdef EMMC_COMPILE
#include "hwlock_com_api.h"
#endif

static cdev_st *g_mmc_dev = NULL;
static struct himci_host *g_host = NULL;
static unsigned long g_write_all_time = 0;
static u64 g_write_all_byte = 0;

#ifdef EMMC_COMPILE
/* Record the register configuration value after the optimal tuning phase in M3 mode. */
static u32 g_lpsc_emmc_ctrl_reg;

static s32 hi_mci_cmd_send(u32 cmd_arg, u32 cmd_flags, u32 cmd_opcode, u32 retries);
#endif

static void hi_mci_sys_reset(const struct himci_host *host)
{
    u32 reg_value;
    ulong flags;

    local_irq_save(flags);

    reg_value = himci_readl(host->base + HISI_MCI_CTRL);
    reg_value |= CTRL_RESET | FIFO_RESET | DMA_RESET;
    himci_writel(reg_value, host->base + HISI_MCI_CTRL);

    reg_value = himci_readl(host->base + HISI_MCI_BMOD);
    reg_value |= BMOD_SWR | BURST_LENGTH | BURST_TYPE;
    himci_writel(reg_value, host->base + HISI_MCI_BMOD);

    local_irq_restore(flags);

    return;
}

static void hi_mci_ctrl_power(const struct himci_host *host, u32 flag)
{
    himci_writel(flag, host->base + HISI_MCI_PWREN);

    return;
}

static u32 hi_mci_sys_card_detect(const struct himci_host *host)
{
    return himci_readl(host->base + HISI_MCI_CDETECT) & HIMCI_CARD0;
}

static u32 hi_mci_ctrl_card_readonly(const struct himci_host *host)
{
    return himci_readl(host->base + HISI_MCI_WRTPRT) & HIMCI_CARD0;
}

static u32 hi_mci_get_relative_addr(ulong phy_addr)
{
    ulong dmac_haddr = 0;
    ulong relative_addr;
    s32 ret;
    u_lpsc_addr_extend0 lpsc_addr_extend0;
    u_lpsc_addr_sel lpsc_addr_sel;

    ret = lp_subsys_reg_read(LP_SUBSYS_CSR_LPSC_ADDR_EXTEND0_OFFSET_ADDR, &lpsc_addr_extend0.val);
    if (ret) {
        return (u32)phy_addr;
    }

    ret = lp_subsys_reg_read(LP_SUBSYS_CSR_LPSC_ADDR_SEL_OFFSET_ADDR, &lpsc_addr_sel.val);
    if (ret) {
        return (u32)phy_addr;
    }

#ifdef EMMC_COMPILE
    switch (lpsc_addr_extend0.bits.emmc_addr_remap_mode) {
        case 0x1:                                                                   /* mode 1 */
            dmac_haddr = (((u64)(lpsc_addr_extend0.bits.emmc_haddr_offset)) << 28); /* left shift 28 */
            break;

        case 0x2:                                                           /* mode 2 */
            dmac_haddr = (((u64)(lpsc_addr_sel.bits.emmc_addr_sel)) << 32); /* left shift 32 */
            break;

        default:
            break;
    }
#else
    switch (lpsc_addr_extend0.bits.sdio_addr_remap_mode) {
        case 0x1:                                                                   /* mode 1 */
            dmac_haddr = (((u64)(lpsc_addr_extend0.bits.sdio_haddr_offset)) << 28); /* left shift 28 */
            break;

        case 0x2:                                                           /* mode 2 */
            dmac_haddr = (((u64)(lpsc_addr_sel.bits.sdio_addr_sel)) << 32); /* left shift 32 */
            break;

        default:
            break;
    }
#endif

    if (phy_addr < dmac_haddr) {
        LOG(LOG_ERROR, "(0x%x) (0x%x)is invalid!", lpsc_addr_extend0.val, lpsc_addr_sel.val);
        return (u32)phy_addr;
    }

    relative_addr = phy_addr - dmac_haddr;
    if ((relative_addr >> 32) != 0) { /* right shift 32 bits */
        LOG(LOG_ERROR, "(0x%x) (0x%x)is invalid!", lpsc_addr_extend0.val, lpsc_addr_sel.val);
        return (u32)phy_addr;
    }

    return (u32)relative_addr;
}

static s32 hi_mci_wait_cmd_tx_success(struct himci_host *host)
{
    unsigned long timeout = jiffies + msecs_to_jiffies(500); /* 500ms */
    u32 reg_data;

    if (host == NULL) {
        return -EIO;
    }

    while (time_before(jiffies, timeout)) {
        /* If START_CMD is 0, the controller has sent the command. */
        reg_data = himci_readl(host->base + HISI_MCI_CMD);
        if ((reg_data & START_CMD) == 0) {
            return 0;
        }

        /* Check whether the HLE_INT_STATUS hardware error is set. */
        reg_data = himci_readl(host->base + HISI_MCI_RINTSTS);
        if (reg_data & HLE_INT_STATUS) {
            himci_writel(HLE_INT_STATUS, host->base + HISI_MCI_RINTSTS);
            LOG(LOG_ERROR, "Error: (HLE)Hardware locked write error,Other CMD is running");
            return -EIO;
        }
    }

    LOG(LOG_WARN, "wait cmd complete is timeout!");

    return -ETIMEDOUT;
}

static void hi_mci_wait_card_idle(struct himci_host *host)
{
    unsigned long timeout = jiffies + msecs_to_jiffies(500); /* 500 ms */

    while (readl(host->base + HISI_MCI_STATUS) & DATA_BUSY) {
        if (time_after(jiffies, timeout)) {
            /* Command will fail; we'll pass error then */
            LOG(LOG_ERROR, "Busy; trying anyway");
            break;
        }
        udelay(10); /* just delay 10 us, then check again */
    }

    return;
}

static void hi_mci_control_cclk(struct himci_host *host, u32 flag)
{
    u32 regs;
    u32 cmdr = HISI_MMC_CMD_UPD_CLK | HISI_MMC_CMD_PRV_DAT_WAIT | HISI_MMC_CMD_START;

    regs = himci_readl(host->base + HISI_MCI_CLKENA);
    if (flag == ENABLE) {
        regs |= CCLK_ENABLE;
    } else {
        regs &= CCLK_DISABLE;
    }
    himci_writel(regs, host->base + HISI_MCI_CLKENA);
    himci_writel(0x0, host->base + HISI_MCI_CMDARG);
    wmb();
    hi_mci_wait_card_idle(host);
    himci_writel(cmdr, host->base + HISI_MCI_CMD);

    if (hi_mci_wait_cmd_tx_success(host)) {
        LOG(LOG_WARN, "disable or enable clk is timeout!!");
    }

    return;
}

static void hi_mci_set_cclk(struct himci_host *host, u32 cclk)
{
    u32 reg_value;
    u32 clk_base;
    u32 cmdr = HISI_MMC_CMD_UPD_CLK | HISI_MMC_CMD_PRV_DAT_WAIT | HISI_MMC_CMD_START;

#ifdef EMMC_COMPILE
    clk_base = EMMC_CLK_BASE;
#else
    clk_base = SDIO_CLK_BASE;
#endif

    reg_value = 0;
    if (cclk < clk_base) {
        reg_value = clk_base / (cclk * 2); /* Clock division value is 2 * N */
        if (clk_base % (cclk * 2)) { /* Clock division value is 2 * N */
            reg_value++;
        }

        if (reg_value > 0xFF) {
            reg_value = 0xFF;
        }
    }
    himci_writel(reg_value, host->base + HISI_MCI_CLKDIV);
    himci_writel(0x0, host->base + HISI_MCI_CMDARG);
    wmb();
    hi_mci_wait_card_idle(host);
    himci_writel(cmdr, host->base + HISI_MCI_CMD);
    if (hi_mci_wait_cmd_tx_success(host) != 0) {
        LOG(LOG_WARN, "set card clk divider is failed!!!");
    }

    return;
}

static void hi_mci_init_card(struct himci_host *host)
{
    u32 tmp_reg;
    ulong flags;

    if (host == NULL) {
        return;
    }

    hi_mci_sys_reset(host);

    hi_mci_ctrl_power(host, POWER_OFF);
    udelay(500); /* delay 500us */

    /* card power on */
    hi_mci_ctrl_power(host, POWER_ON);
    udelay(200); /* delay 200us */

    /* clear MMC host intr */
    himci_writel(ALL_INT_CLR, host->base + HISI_MCI_RINTSTS);

    spin_lock_irqsave(&host->lock, flags);
    host->pending_events = 0;
    spin_unlock_irqrestore(&host->lock, flags);

    /* MASK MMC host intr */
    tmp_reg = himci_readl(host->base + HISI_MCI_INTMASK);
    tmp_reg &= ~ALL_INT_MASK;
    tmp_reg |= DTO_INT_MASK;
    himci_writel(tmp_reg, host->base + HISI_MCI_INTMASK);

    /* enable inner DMA mode and close intr of MMC host controler */
    tmp_reg = himci_readl(host->base + HISI_MCI_CTRL);
    tmp_reg |= USE_INNER_DMA | INTR_EN;
    himci_writel(tmp_reg, host->base + HISI_MCI_CTRL);

    /* set timeout param */
    himci_writel(DATA_TIMEOUT | RESPONSE_TIMEOUT, host->base + HISI_MCI_TIMEOUT);

    /* set FIFO param */
    tmp_reg = BURST_SIZE | RX_WMARK | TX_WMARK;
    himci_writel(tmp_reg, host->base + HISI_MCI_FIFOTH);

    /* set cardthrtl */
    tmp_reg = CARD_READ_THRESHHOLD | BUSY_CLEAR_INT_ENABLE | CARD_READ_THR_EN;
    himci_writel(tmp_reg, host->base + HISI_MCI_CARDTHRCTL);

    return;
}

#ifndef EMMC_COMPILE
static void hi_mci_detect_card(struct timer_list *tl)
{
    struct himci_host *host = from_timer(host, tl, timer);
    u32 i;
    u32 curr_status;
    u32 status[3]; /* need get 3 times status to check */
    u32 detect_retry_count = 0;

    if (host == NULL) {
        return;
    }

    /* De-shake */
    for (;;) {
        for (i = 0; i < 3; i++) { /* read 3 times, make sure value is the same */
            status[i] = hi_mci_sys_card_detect(host);
            udelay(1);
        }

        /* Values 0, 1, and 2 are read every 1 μs, if they are the same, no jitter occurs. */
        if ((status[0] == status[1]) && (status[0] == status[2])) {
            break;
        }

        detect_retry_count++;
        if (detect_retry_count >= MAX_RETRY_COUNT) {
            LOG(LOG_ERROR, "this is a dithering,card detect error!");
            goto err;
        }
    }

    curr_status = status[0];
    if (curr_status != host->card_status) {
        host->card_status = curr_status;
        if (curr_status != CARD_UNPLUGED) {
            hi_mci_init_card(host);
            LOG(LOG_INFO, "card connected!");
        } else {
            LOG(LOG_INFO, "card disconnected!");
        }

        mmc_detect_change(host->mmc, 0);
    }

err:
    mod_timer(&host->timer, jiffies + HI_MCI_DETECT_TIMEOUT);

    return;
}
#endif

static void hi_mci_idma_start(const struct himci_host *host)
{
    u32 tmp;
    dma_addr_t dma_addr;

    dma_addr = hi_mci_get_relative_addr(host->dma_paddr);
    himci_writel(dma_addr, host->base + HISI_MCI_DBADDR);
    tmp = himci_readl(host->base + HISI_MCI_BMOD);
    tmp |= BMOD_DMA_EN;
    himci_writel(tmp, host->base + HISI_MCI_BMOD);

    return;
}

static void hi_mci_idma_stop(const struct himci_host *host)
{
    u32 tmp_reg;

    tmp_reg = himci_readl(host->base + HISI_MCI_BMOD);
    tmp_reg &= ~BMOD_DMA_EN;
    himci_writel(tmp_reg, host->base + HISI_MCI_BMOD);

    return;
}

static s32 hi_mci_setup_data(struct himci_host *host, struct mmc_data *data)
{
    u32 sg_length;
    ulong sg_phyaddr;
    u32 i;
    u32 max_des = (MMC_MAX_PAGE_COUNT * PAGE_SIZE) / sizeof(struct himci_des);
    u32 desc_cnt = 0;
    struct himci_des *des = NULL;

    host->data = data;

    if (data->flags & MMC_DATA_READ) {
        host->dma_dir = DMA_FROM_DEVICE;
    } else {
        g_write_all_byte = g_write_all_byte + data->blksz * data->blocks;
        host->dma_dir = DMA_TO_DEVICE;
    }

    if ((data->blksz * data->blocks) > (DMA_BUFFER * max_des)) {
        LOG(LOG_ERROR, "mci request data_size is too big!");
        return -ENOMEM;
    }

    host->dma_sg = data->sg;
    host->dma_sg_num = (u32)dma_map_sg(mmc_dev(host->mmc), data->sg,
                                       (s32)data->sg_len, host->dma_dir);
    if (host->dma_sg_num == 0) {
        return -EIO;
    }

    des = (struct himci_des *)host->dma_vaddr;

    for (i = 0; i < host->dma_sg_num; i++) {
        sg_length = sg_dma_len(&data->sg[i]);
        sg_phyaddr = sg_dma_address(&data->sg[i]);
        while (sg_length) {
            des[desc_cnt].idmac_des_ctrl = DMA_DES_OWN | DMA_DES_NEXT_DES;
            des[desc_cnt].idmac_des_buf_addr = hi_mci_get_relative_addr(sg_phyaddr);
            /* idmac_des_next_addr is paddr for dma */
            des[desc_cnt].idmac_des_next_addr = hi_mci_get_relative_addr(host->dma_paddr +
                                                                         (desc_cnt + 1) * sizeof(struct himci_des));
            if (sg_length >= 0x1000) {
                des[desc_cnt].idmac_des_buf_size = 0x1000;
                sg_length -= 0x1000;
                sg_phyaddr += 0x1000;
            } else {
                /* data alignment */
                des[desc_cnt].idmac_des_buf_size = sg_length;
                sg_length = 0;
            }
            desc_cnt++;
        }
    }

    des[0].idmac_des_ctrl |= DMA_DES_FIRST_DES;
    des[desc_cnt - 1].idmac_des_ctrl |= DMA_DES_LAST_DES;
    des[desc_cnt - 1].idmac_des_next_addr = 0;

    return 0;
}

static s32 hi_mci_exec_cmd(struct himci_host *host, struct mmc_command *cmd, struct mmc_data *data)
{
    u32 cmdr = (cmd->opcode & 0x3f) | HISI_MMC_CMD_USE_HOLD_REG;

    host->cmd = cmd;
    if (data) {
        cmdr |= HISI_MMC_CMD_DAT_EXP;

#if LINUX_VERSION_CODE < KERNEL_VERSION(5, 10, 0)
        if (data->flags & MMC_DATA_STREAM) {
            cmdr |= HISI_MMC_CMD_STRM_MODE;
        }
#endif

        if (data->flags & MMC_DATA_WRITE) {
            cmdr |= HISI_MMC_CMD_DAT_WR;
        }

        if (!host->mrq->stop && (data->blocks > 1)) {
            cmdr |= HISI_MMC_CMD_SEND_STOP;
        }
    }

    if (cmd == host->mrq->stop) {
        cmdr |= HISI_MMC_CMD_STOP;
    } else {
        cmdr |= HISI_MMC_CMD_PRV_DAT_WAIT;
    }

    switch (mmc_resp_type(cmd)) {
        case MMC_RSP_NONE:
            break;
        case MMC_RSP_R1:
        case MMC_RSP_R1B:
            cmdr |= (HISI_MMC_CMD_RESP_EXP | HISI_MMC_CMD_RESP_CRC);
            break;
        case MMC_RSP_R2:
            cmdr |= (HISI_MMC_CMD_RESP_EXP | HISI_MMC_CMD_RESP_CRC | HISI_MMC_CMD_RESP_LONG);
            break;
        case MMC_RSP_R3:
        case MMC_RSP_R1_NO_CRC:
            cmdr |= HISI_MMC_CMD_RESP_EXP;
            break;
        default:
            LOG(LOG_ERROR, "Error type 0x%x, opcode 0x%x", mmc_resp_type(cmd), cmd->opcode);
    }

    if (cmd->opcode == MMC_GO_IDLE_STATE) {
        cmdr |= HISI_MMC_CMD_INIT;
    }

    himci_writel(cmd->arg, host->base + HISI_MCI_CMDARG);
    wmb();
    cmdr |= HISI_MMC_CMD_START;

    hi_mci_wait_card_idle(host);

    himci_writel(cmdr, host->base + HISI_MCI_CMD);
    if (hi_mci_wait_cmd_tx_success(host) != 0) {
        return -EINVAL;
    }

    return 0;
}

static void hi_mci_finish_request(struct himci_host *host,
                                  struct mmc_request *mrq)
{
    if ((host == NULL) || (mrq == NULL)) {
        return;
    }

    host->mrq = NULL;
    host->cmd = NULL;
    host->data = NULL;
    mmc_request_done(host->mmc, mrq);

    return;
}

static void hi_mci_cmd_done(struct himci_host *host, u32 stats)
{
    u32 i;
    struct mmc_command *cmd = host->cmd;

    /* Four RESP registers need to be read. */
    for (i = 0; i < 4; i++) {
        if (mmc_resp_type(cmd) == MMC_RSP_R2) {
            cmd->resp[i] = himci_readl(host->base + HISI_MCI_RESP3 - i * 0x4);
        } else {
            cmd->resp[i] = himci_readl(host->base + HISI_MCI_RESP0 + i * 0x4);
        }
    }

    /* Test command response error. */
    if (stats & RTO_INT_STATUS) {
        cmd->error = -ETIMEDOUT;
        LOG(LOG_ERROR, "TIMEOUT: (RTO)Response timeout");
    }

    if (stats & RE_INT_STATUS) {
        cmd->error = -EILSEQ;
        LOG(LOG_ERROR, "Error: (RE)Response error");
    }

    if (stats & RCRC_INT_STATUS) {
        cmd->error = -EILSEQ;
        LOG(LOG_ERROR, "Error: (RCRC)Response CRC error");
    }

    host->cmd = NULL;

    return;
}

static void hi_mci_data_done(struct himci_host *host, u32 stats, u32 flag)
{
    struct mmc_data *data = host->data;

    if (data == NULL) {
        LOG(LOG_ERROR, "data is null pointer!");
        return;
    }

    if (stats & HTO_INT_STATUS) {
        data->error = -ETIMEDOUT;
        LOG(LOG_ERROR, "TIMEOUT: (HTO)Data starvation by host timeout");
    }

    if (stats & DRTO_INT_STATUS) {
        data->error = -ETIMEDOUT;
        LOG(LOG_ERROR, "TIMEOUT: (DRTO)Data read timeout");
    }

    if (stats & EBE_INT_STATUS) {
        data->error = -EILSEQ;
        LOG(LOG_ERROR, "Error: (EBE)End bit error");
    }

    if ((stats & SBE_INT_STATUS) && (flag & MMC_DATA_READ)) {
        data->error = -EILSEQ;
        LOG(LOG_ERROR, "Error: (SBE)Start bit error");
    }

    if (stats & FRUN_INT_STATUS) {
        data->error = -EILSEQ;
        LOG(LOG_ERROR, "Error: (FRUN)FIFO underrun/overrun error");
    }

    if (stats & DCRC_INT_STATUS) {
        data->error = -EILSEQ;
        LOG(LOG_ERROR, "Error: (DCRC)Data CRC error");
    }

    /* Record the number of bytes transferred */
    if (!data->error) {
        data->bytes_xfered = data->blocks * data->blksz;
    } else {
        data->bytes_xfered = 0;
    }

    host->data = NULL;

    return;
}

static s32 hi_mci_wait_cmd_complete(struct himci_host *host)
{
    ulong cmd_jiffies_timeout;
    u32 cmd_irq_reg;
    struct mmc_command *cmd = host->cmd;

    if (cmd == NULL) {
        LOG(LOG_ERROR, "cmd is null pointer!");
        return -EIO;
    }

    cmd_jiffies_timeout = jiffies + HI_MCI_REQUEST_TIMEOUT;
    while (time_before(jiffies, cmd_jiffies_timeout)) {
        cmd_irq_reg = himci_readl(host->base + HISI_MCI_RINTSTS);
        if (cmd_irq_reg & CD_INT_STATUS) {
            himci_writel((CD_INT_STATUS | RTO_INT_STATUS |
                            RCRC_INT_STATUS | RE_INT_STATUS),
                            host->base + HISI_MCI_RINTSTS);
            hi_mci_cmd_done(host, cmd_irq_reg);
            return 0;
        }

        schedule();
    }

    cmd->error = -ETIMEDOUT;
    LOG(LOG_ERROR, "wait cmd request complete is timeout!");
    return -ETIMEDOUT;
}

static s32 hi_mci_wait_data_complete(struct himci_host *host, u32 flag)
{
    u32 data_irq_reg;
    s32 ret = 0;
    struct mmc_data *data = host->data;
    long time;
    ulong flags;
    u32 retry_count = MAX_RETRY_COUNT;

    if (data == NULL) {
        LOG(LOG_ERROR, "data is null pointer!");
        return -EIO;
    }

    time = wait_event_timeout(host->intr_wait, test_bit(HIMCI_PEND_DTO_B, &host->pending_events),
                              HI_MCI_REQUEST_TIMEOUT);
    if ((time <= 0) && (!test_bit(HIMCI_PEND_DTO_B, &host->pending_events))) {
        data->error = -ETIMEDOUT;
        ret = -ETIMEDOUT;
    }

    data_irq_reg = himci_readl(host->base + HISI_MCI_RINTSTS);

    if ((ret == 0) && (flag & MMC_DATA_WRITE)) {
        do {
#ifdef EMMC_COMPILE
            if (himci_readl(host->base + HISI_MCI_RINTSTS) & SBE_INT_STATUS) {
#else
            if (!(himci_readl(host->base + HISI_MCI_STATUS) & DATA_BUSY)) {
#endif
                break;
            }
            mdelay(1); /* wait 1 ms */
            retry_count--;
        } while (retry_count);

        if (retry_count == 0) {
            LOG(LOG_ERROR, "card busy timeout!");
            ret = -ETIMEDOUT;
        }
    }

    himci_writel((HTO_INT_STATUS | DRTO_INT_STATUS | EBE_INT_STATUS | SBE_INT_STATUS
                  | FRUN_INT_STATUS | DCRC_INT_STATUS), host->base + HISI_MCI_RINTSTS);

    spin_lock_irqsave(&host->lock, flags);
    host->pending_events &= ~HIMCI_PEND_DTO_M;
    spin_unlock_irqrestore(&host->lock, flags);

    hi_mci_data_done(host, data_irq_reg, flag);

    return ret;
}

static s32 hi_mci_wait_dma_complete(struct himci_host *host)
{
    ulong card_jiffies_timeout;
    struct himci_des *des = NULL;

    if (host == NULL) {
        return -EIO;
    }

    des = (struct himci_des *)host->dma_vaddr;
    card_jiffies_timeout = jiffies + HI_MCI_REQUEST_TIMEOUT;

    while (time_before(jiffies, card_jiffies_timeout)) {
        if (!(des[0].idmac_des_ctrl & DMA_DONE)) {
            return 0;
        }
    }

    return -ETIMEDOUT;
}

static s32 hi_mci_get_hw_lock(void)
{
#ifdef EMMC_COMPILE
    s32 ret;

    u32 retry_num = EMMC_GET_HW_LOCK_RETRY_COUNT;
    do {
        ret = hw_acquire_hard_lock(EMMC_CTLR_LOCK_ID);
        if (ret == 0) {
            break;
        }
        msleep(1); /* sleep 1 ms */
    } while (retry_num--);

    return ret;
#else
    return 0;
#endif
}

static s32 hi_mci_prepare_data(struct himci_host *host, struct mmc_request *mrq)
{
    u32 tmp_reg;
    u32 blk_size;
    u32 fifo_count = 0;
    s32 ret;
    ulong flags;

    if (!mrq->data) {
        himci_writel(0, host->base + HISI_MCI_BYTCNT);
        himci_writel(0, host->base + HISI_MCI_BLKSIZ);
        return 0;
    }

    /* prepare data */
    ret = hi_mci_setup_data(host, mrq->data);
    if (ret) {
        mrq->data->error = ret;
        LOG(LOG_WARN, "data setup is error(%d)!", ret);
        return ret;
    }

    blk_size = mrq->data->blksz * mrq->data->blocks;
    himci_writel(blk_size, host->base + HISI_MCI_BYTCNT);
    himci_writel(mrq->data->blksz, host->base + HISI_MCI_BLKSIZ);

    tmp_reg = himci_readl(host->base + HISI_MCI_CTRL);
    tmp_reg |= (FIFO_RESET | USE_INNER_DMA);
    himci_writel(tmp_reg, host->base + HISI_MCI_CTRL);

    do {
        tmp_reg = himci_readl(host->base + HISI_MCI_CTRL);
        fifo_count++;
        if (fifo_count >= MAX_RETRY_COUNT) {
            LOG(LOG_ERROR, "fifo reset is timeout!");
            hi_mci_idma_stop(host);
            dma_unmap_sg(mmc_dev(host->mmc), mrq->data->sg, (s32)mrq->data->sg_len, host->dma_dir);
            /* clear MMC host intr */
            spin_lock_irqsave(&host->lock, flags);
            himci_writel(ALL_INT_CLR, host->base + HISI_MCI_RINTSTS);
            spin_unlock_irqrestore(&host->lock, flags);

            return -ETIMEDOUT;
        }
    } while (tmp_reg & FIFO_RESET);

    /* start DMA */
    hi_mci_idma_start(host);

    return 0;
}

static void hi_mci_send_data(struct himci_host *host, struct mmc_request *mrq)
{
    s32 ret;

    /* send command */
    ret = hi_mci_exec_cmd(host, mrq->cmd, mrq->data);
    if (ret) {
        mrq->cmd->error = ret;
        LOG(LOG_ERROR, "cmd execute is error!");
        return;
    }

    /* wait command send complete */
    ret = hi_mci_wait_cmd_complete(host);
    if (ret) {
        return;
    }

    if (!(mrq->data && !mrq->cmd->error)) {
        return;
    }

    /* start data transfer */
    if (mrq->data) {
        /* wait data transfer complete */
        ret = hi_mci_wait_data_complete(host, mrq->data->flags);
        if (ret) {
            return;
        }

        ret = hi_mci_wait_dma_complete(host);
        if (ret) {
            return;
        }

        if (mrq->stop) {
            /* send stop command */
            ret = hi_mci_exec_cmd(host, host->mrq->stop, host->data);
            if (ret) {
                mrq->cmd->error = (u32)ret;
                return;
            }

            ret = hi_mci_wait_cmd_complete(host);
            if (ret) {
                return;
            }
        }
    }

    return;
}

static void hi_mci_request(struct mmc_host *mmc, struct mmc_request *mrq)
{
    struct himci_host *host = (struct himci_host *)mmc_priv(mmc);
    s32 ret;
    ulong flags;

    if (mrq == NULL) {
        return;
    }

    /* Obtain the hardware lock. */
    ret = hi_mci_get_hw_lock();
    if (ret) {
        mrq->cmd->error = -EILSEQ;
        hi_mci_finish_request(host, mrq);
        LOG(LOG_WARN, "Warning:get_hard_lock failed!");
        return;
    }

    mutex_lock(&host->m_lock);

    host->mrq = mrq;
    if (host->card_status == CARD_UNPLUGED) {
        mrq->cmd->error = -ENODEV;
        goto REQUEST_DONE;
    }

    ret = hi_mci_prepare_data(host, mrq);
    if (ret) {
        goto REQUEST_DONE;
    }

    hi_mci_send_data(host, mrq);

    if (mrq->data) {
        hi_mci_idma_stop(host);
        dma_unmap_sg(mmc_dev(host->mmc), mrq->data->sg, (s32)mrq->data->sg_len,
                     host->dma_dir);
    }

    /* clear MMC host intr */
    spin_lock_irqsave(&host->lock, flags);
    himci_writel(ALL_INT_CLR, host->base + HISI_MCI_RINTSTS);
    spin_unlock_irqrestore(&host->lock, flags);
REQUEST_DONE:
    hi_mci_wait_card_idle(host);
    hi_mci_finish_request(host, mrq);

    mutex_unlock(&host->m_lock);
    /* Release the hardware lock. */
#ifdef EMMC_COMPILE
    hw_release_hard_lock(EMMC_CTLR_LOCK_ID);
#endif
    return;
}

static s32 mmc_phase_config(u32 mode, u32 value)
{
    u_lpsc_emmc_func_ctrl lp_emmc_ctrl = {.val = 0};
    u32 drv_ps_sel;

    switch (mode) {
        case MMC_COMPATIBILITY_MODE:
            lp_emmc_ctrl.bits.sc_sdio_drv_ps_sel = 0x4;
            lp_subsys_reg_write(LP_SUBSYS_CSR_LPSC_EMMC_FUNC_CTRL_OFFSET_ADDR, lp_emmc_ctrl.val);
            return 0;
        case MMC_HS_SDR_MODE:
        case MMC_HS_DDR_MODE:
            lp_emmc_ctrl.bits.sc_sdio_sap_ps_sel = 0x1;
            lp_emmc_ctrl.bits.sc_sdio_sap_dll_dllmode = 0x1;
            lp_emmc_ctrl.bits.sc_sdio_sap_dll_slave_en = 0x1;
            lp_emmc_ctrl.bits.sc_sdio_sap_dll_dll_ssel = 0xf;
            break;
        case MMC_HS200_SDR_MODE:
            lp_emmc_ctrl.bits.sc_sdio_sap_ps_sel = value;
            break;
        case MMC_HS400_DDR_MODE:
            lp_emmc_ctrl.bits.sc_sdio_sap_ps_sel = 0x4;
            lp_emmc_ctrl.bits.sc_sdio_sap_dll_slave_en = 0x1;
            lp_emmc_ctrl.bits.sc_sdio_sap_dll_dllmode = 0x1;
            lp_emmc_ctrl.bits.sc_sdio_sap_dll_dll_ssel = 0x2;
            break;
        default:
            LOG(LOG_WARN, "mode(%u) error", mode);
            return -EFAULT;
    }

    /*
     * Check whether the eMMC frequency is divided.
     * If yes, set the clock phase to 0x4 (180 degrees). Otherwise, set the clock phase to 2 (90 degrees).
     */
    drv_ps_sel = (readl(g_host->base + HISI_MCI_CLKDIV)) ? 0x4 : 0x2;
    lp_emmc_ctrl.bits.sc_sdio_drv_ps_sel  = drv_ps_sel;

    return lp_subsys_reg_write(LP_SUBSYS_CSR_LPSC_EMMC_FUNC_CTRL_OFFSET_ADDR, lp_emmc_ctrl.val);
}

#ifdef EMMC_COMPILE
static s32 hi_mci_hs200_tuning(struct mmc_host *mmc, u32 opcode)
{
    /* restore phase config */
    return lp_subsys_reg_write(LP_SUBSYS_CSR_LPSC_EMMC_FUNC_CTRL_OFFSET_ADDR, g_lpsc_emmc_ctrl_reg);
}
#endif

/*
 * Provide the kernel with the ability to set the clock width power supply.
 */
static void hi_mci_set_ios(struct mmc_host *mmc, struct mmc_ios *ios)
{
    struct himci_host *host = (struct himci_host *)mmc_priv(mmc);
    u32 tmp_reg;

    if ((host == NULL) || (ios == NULL)) {
        return;
    }

    switch (ios->power_mode) {
        case MMC_POWER_OFF:
            hi_mci_ctrl_power(host, POWER_OFF);
            break;
        case MMC_POWER_UP:
        case MMC_POWER_ON:
            hi_mci_ctrl_power(host, POWER_ON);
            break;
        default:
            break;
    }

    if (ios->clock) {
        hi_mci_control_cclk(host, DISABLE);
        hi_mci_set_cclk(host, ios->clock);
        hi_mci_control_cclk(host, ENABLE);
    } else {
        hi_mci_control_cclk(host, DISABLE);
    }

    /* clear bus width to 1bit first */
    tmp_reg = himci_readl(host->base + HISI_MCI_CTYPE);
    tmp_reg &= ~(CARD_WIDTH_0 | CARD_WIDTH_1);

    if (MMC_BUS_WIDTH_8 == ios->bus_width) {
        tmp_reg |= CARD_WIDTH_0;
    } else if (MMC_BUS_WIDTH_4 == ios->bus_width) {
        tmp_reg |= CARD_WIDTH_1;
    }

    himci_writel(tmp_reg, host->base + HISI_MCI_CTYPE);

    if (ios->timing == MMC_TIMING_MMC_DDR52) {
        mmc_phase_config(MMC_HS_DDR_MODE, 0);
        /* 0x10000: enables the DDR mode. */
        himci_writel(0x10000, host->base + HISI_MCI_UHS_REG);
    } else if (ios->timing == MMC_TIMING_MMC_HS) {
        mmc_phase_config(MMC_HS_SDR_MODE, 0);
        himci_writel(0x0, host->base + HISI_MCI_UHS_REG);
    } else {
        himci_writel(0x0, host->base + HISI_MCI_UHS_REG);
    }

    return;
}

static s32 hi_mci_get_ro(struct mmc_host *mmc)
{
    struct himci_host *host = (struct himci_host *)mmc_priv(mmc);

    return hi_mci_ctrl_card_readonly(host);
}

static void hi_mci_hw_reset(struct mmc_host *mmc)
{
    struct himci_host *host = (struct himci_host *)mmc_priv(mmc);

    hi_mci_init_card(host);

    return;
}

#ifdef EMMC_COMPILE
static s32 hi_mci_send_cmd(struct mmc_request *mrq)
{
    s32 ret;

    g_host->mrq = mrq;
    /* send command */
    ret = hi_mci_exec_cmd(g_host, mrq->cmd, NULL);
    if (ret) {
        mrq->cmd->error = ret;
        return ret;
    }

    /* wait command send complete */
    return hi_mci_wait_cmd_complete(g_host);
}

static s32 hi_mci_mmc_extcsdreg_get(u32 offset, u32 count, u8 __user *buf)
{
    s32 ret = 0;
    u8 *tmpbuf = NULL;
    u8 *ext_csd = NULL;
    u32 i = 0;

    tmpbuf = (u8 *)kzalloc(count, GFP_KERNEL);
    if (tmpbuf == NULL) {
        LOG(LOG_ERROR, "kmalloc err");
        ret = -ENOMEM;
        goto exit;
    }

    mmc_claim_host(g_host->mmc);
    ret = mmc_get_ext_csd(g_host->mmc->card, &ext_csd);
    mmc_release_host(g_host->mmc);
    if (ret) {
        LOG(LOG_ERROR, "send cmd for ext_csd reg fail");
        goto exit;
    }

    while (i < count) {
        *(tmpbuf + i) = ext_csd[offset + i];
        i++;
    }

    if (copy_to_user(buf, tmpbuf, count)) {
        LOG(LOG_ERROR, "ERROR copy_to_user");
        ret = -EFAULT;
    }

    if (ext_csd != NULL) {
        kfree(ext_csd);
    }
exit:
    if (tmpbuf != NULL) {
        kfree(tmpbuf);
    }

    return ret;
}

static s32 hi_mci_mmc_reg_get_ext_csd(u32 offset, u32 count, u8 __user *buf)
{
    s32 ret;

    if ((offset > MAX_EXT_CSD_OFFSET) || (count > EXT_CSD_COUNT) ||
        ((count + offset) > (MAX_EXT_CSD_OFFSET + 1))) {
        LOG(LOG_ERROR, "ext csd reg offset or reg length is out of range!");
        return -EINVAL;
    }

    ret = hi_mci_mmc_extcsdreg_get(offset, count, buf);
    if (ret) {
        LOG(LOG_ERROR, "get ext csd reg fail");
        return ret;
    }

    return 0;
}

static s32 hi_mci_mmc_reg_get_cid(u32 offset, u32 count, u8 __user *buf)
{
    if ((offset != CID_OFFSET) || (count != CID_COUNT)) {
        LOG(LOG_ERROR, "cid reg offset or reg length is out of range!");
        return -EINVAL;
    }

    if (copy_to_user(buf, (u8 *)g_host->mmc->card->raw_cid, count)) {
        LOG(LOG_ERROR, "ERROR copy_to_user");
        return -EFAULT;
    }

    return 0;
}

static s32 hi_mci_mmc_reg_get_csd(u32 offset, u32 count, u8 __user *buf)
{
    if ((offset != CSD_OFFSET) || (count != CSD_COUNT)) {
        LOG(LOG_ERROR, "csd reg offset or reg length is out of range!");
        return -EINVAL;
    }

    if (copy_to_user(buf, (u8 *)g_host->mmc->card->raw_csd, count)) {
        LOG(LOG_ERROR, "ERROR copy_to_user");
        return -EFAULT;
    }

    return 0;
}

static s32 hi_mci_mmc_reg_get(mmc_reg_type_s type, u32 offset, u32 count, u8 __user *buf)
{
    s32 ret = 0;

    if ((EXT_CSD != type) && (CID != type) && (CSD != type)) {
        LOG(LOG_ERROR, "reg type(%u) is not matched", type);
        return -EINVAL;
    }

    if (buf == NULL) {
        LOG(LOG_ERROR, "buf is null!");
        return -EINVAL;
    }

    /* The CID and CSD are obtained during kernel initialization. */
    switch (type) {
        case MMC_EXT_CSD:
            ret = hi_mci_mmc_reg_get_ext_csd(offset, count, buf);
            break;
        case MMC_CID:
            ret = hi_mci_mmc_reg_get_cid(offset, count, buf);
            break;
        case MMC_CSD:
            ret = hi_mci_mmc_reg_get_csd(offset, count, buf);
            break;
    }

    return ret;
}

static s32 hi_mci_cmd_send(u32 cmd_arg, u32 cmd_flags, u32 cmd_opcode, u32 retries)
{
    s32 ret;
    struct mmc_command cmd = { 0 };

    if ((cmd_opcode > MAX_CMD)) {
        LOG(LOG_ERROR, "cmd opcode(%u) is invalid or response is NULL!!", cmd_opcode);
        return -EINVAL;
    }

    cmd.opcode = cmd_opcode;
    cmd.arg = cmd_arg;
    cmd.flags = cmd_flags;

    if (((mmc_resp_type(&cmd) != MMC_RSP_NONE) && (mmc_resp_type(&cmd) != MMC_RSP_R1)
         && (mmc_resp_type(&cmd) != MMC_RSP_R1B) && (mmc_resp_type(&cmd) != MMC_RSP_R2)
         && (mmc_resp_type(&cmd) != MMC_RSP_R3)) || ((mmc_cmd_type(&cmd) != MMC_CMD_AC)
         && (mmc_cmd_type(&cmd) != MMC_CMD_ADTC) && (mmc_cmd_type(&cmd) != MMC_CMD_ADTC)
         && (mmc_cmd_type(&cmd) != MMC_CMD_BC) && (mmc_cmd_type(&cmd) != MMC_CMD_BCR))) {
        LOG(LOG_ERROR, "cmd flags is invalid!!");
        return -EINVAL;
    }

    ret = mmc_wait_for_cmd(g_host->mmc, &cmd, retries);
    if (ret) {
        LOG(LOG_ERROR, "send mmc cmd fail");
    }

    return ret;
}

/*
 * Enable or clear write protection for a single write protection group based on the parameters.
 */
static s32 hi_mci_tmp_write_protect(u8 enable, u32 addr)
{
    s32 ret;
    u32 retry_count = EMMC_SEND_CMD_RETRY_COUNT;
    struct mmc_command mmc_wr_cmd = { 0 };
    struct mmc_request mrq = { 0 };

    if (enable) {
        mmc_wr_cmd.opcode = MMC_SET_WRITE_PROT;
    } else {
        mmc_wr_cmd.opcode = MMC_CLR_WRITE_PROT;
    }

    mmc_wr_cmd.arg = addr;
    mmc_wr_cmd.flags = MMC_RSP_R1B;
    mrq.cmd = &mmc_wr_cmd;

    ret = hi_mci_send_cmd(&mrq);
    if (ret) {
        LOG(LOG_ERROR, "send cmd(0x%x) failed", mmc_wr_cmd.opcode);
        return ret;
    }

    (void)memset_s(&mmc_wr_cmd, sizeof(struct mmc_command), 0, sizeof(struct mmc_command));

    mmc_wr_cmd.opcode = MMC_SEND_STATUS;
    mmc_wr_cmd.flags = MMC_RSP_R1;
    mmc_wr_cmd.arg = g_host->mmc->card->rca << 16; /* rca addr need shift left 16 bits */
    mrq.cmd = &mmc_wr_cmd;

    do {
        ret = hi_mci_send_cmd(&mrq);
        if (ret) {
            LOG(LOG_ERROR, "send cmd(0x%x) failed", mmc_wr_cmd.opcode);
            return ret;
        }

        if ((mmc_wr_cmd.resp[0] & MMC_STATUS_RDY_FOR_DATA)
            && (mmc_wr_cmd.resp[0] & MMC_STATUS_CURR_STATE) != MMC_STATE_PRG) {
            break;
        } else if (mmc_wr_cmd.resp[0] & MMC_STATUS_MASK) {
            LOG(LOG_ERROR, "cmd status error: 0x%x", mmc_wr_cmd.resp[0]);
            return -EIO;
        }
        udelay(10);
        retry_count--;
    } while (retry_count > 0);

    if (retry_count == 0) {
        LOG(LOG_ERROR, "hi_mci_tmp_write_protect TIMEOUT");
        return -ETIMEDOUT;
    }

    return ret;
}

static s32 hi_mci_mmc_switch(u8 index, u8 value)
{
    s32 ret;
    u32 retry_count = EMMC_SEND_CMD_RETRY_COUNT;
    struct mmc_command mmc_cmd = { 0 };
    struct mmc_request mrq = { 0 };

    mmc_cmd.opcode = MMC_SWITCH;
    mmc_cmd.flags = MMC_RSP_R1B;
    /* switch mode is 24-31bit，index is 16-23bit，value is 8-15bit */
    mmc_cmd.arg = (MMC_SWITCH_MODE_WRITE_BYTE << 24) | (index << 16) | (value << 8);

    mrq.cmd = &mmc_cmd;
    ret = hi_mci_send_cmd(&mrq);
    if (ret) {
        LOG(LOG_ERROR, "send cmd(0x%x) failed", mmc_cmd.opcode);
        return ret;
    }

    (void)memset_s(&mmc_cmd, sizeof(mmc_cmd), 0, sizeof(mmc_cmd));

    mmc_cmd.opcode = MMC_SEND_STATUS;
    mmc_cmd.flags = MMC_RSP_R1;
    mmc_cmd.arg = g_host->mmc->card->rca << 16; /* left shift 16 bits */
    mrq.cmd = &mmc_cmd;

    do {
        ret = hi_mci_send_cmd(&mrq);
        if (ret) {
            LOG(LOG_ERROR, "send cmd(0x%x) failed", mmc_cmd.opcode);
            return ret;
        }

        if ((mmc_cmd.resp[0] & MMC_STATUS_RDY_FOR_DATA)
            && (mmc_cmd.resp[0] & MMC_STATUS_CURR_STATE) != MMC_STATE_PRG) {
            break;
        } else if (mmc_cmd.resp[0] & MMC_STATUS_MASK) {
            LOG(LOG_ERROR, "Resp status error: 0x%x", mmc_cmd.resp[0]);
            return -EIO;
        }
        udelay(1);
    } while (retry_count--);

    if (retry_count == 0) {
        LOG(LOG_ERROR, "mmc_switch TIMEOUT");
        return -ETIMEDOUT;
    }

    return ret;
}

static s32 hi_mci_write_protect_set(u8 type, u8 *ext_csd, u8 area_index)
{
    s32 ret;
    u32 addr;
    u32 i;
    u32 wp_gp_size, wp_gp_count;
    u8 gpp_index, current_area;

    current_area = ext_csd[EXT_CSD_PART_CONFIG];

    ret = hi_mci_mmc_switch(EXT_CSD_PART_CONFIG, area_index);
    if (ret) {
        LOG(LOG_ERROR, "switch area(0x%x) error", area_index);
        return ret;
    }

    /* Calculate the number of protection groups. */
    gpp_index = area_index - GPP0_AREA_INDEX;
    wp_gp_count = ((ext_csd[EXT_CSD_GP_SIZE_MULT + 0 + 3 * gpp_index]) +
                   (ext_csd[EXT_CSD_GP_SIZE_MULT + 1 + 3 * gpp_index] << 8) +
                   (ext_csd[EXT_CSD_GP_SIZE_MULT + 2 + 3 * gpp_index] << 16));

    /*
     * Indicates the size of a protection group. The unit is block (512 bytes).
     * The formula is as follows: 1024*wp_grp_size*erase_frp_size.
     */
    wp_gp_size = 1024 * ext_csd[EXT_CSD_HC_WP_GRP_SIZE] * ext_csd[EXT_CSD_HC_ERASE_GRP_SIZE];

    /* Set write protection for a write protection group cyclically. */
    for (i = 0; i < wp_gp_count; i++) {
        addr = i * wp_gp_size;
        ret = hi_mci_tmp_write_protect(type, addr);
        if (ret) {
            LOG(LOG_ERROR, "Addr[%u] set/clear protect error", addr);
            return ret;
        }
    }

    /* Cut back to the original zone */
    ret = hi_mci_mmc_switch(EXT_CSD_PART_CONFIG, current_area);
    if (ret) {
        LOG(LOG_ERROR, "switch area(0x%x) error", current_area);
        return ret;
    }

    return 0;
}

/*
 * Clear GPP partition write protection and enable temporary write protection.
 */
static s32 hi_mci_gpp_write_protect(u8 type, u8 area_index)
{
    s32 ret;
    u8 *ext_csd = NULL;

    /* Check the validity of the partition. */
    if ((area_index > EMMC_AREA_MAX_NUM) || (area_index < GPP0_AREA_INDEX)) {
        LOG(LOG_ERROR, "area(0x%x) not gpp", area_index);
        return -EINVAL;
    }

    /* Read the extended CSD register. */
    mmc_claim_host(g_host->mmc);
    ret = mmc_get_ext_csd(g_host->mmc->card, &ext_csd);
    if (ret) {
        LOG(LOG_ERROR, "get ext_csd failed");
        mmc_release_host(g_host->mmc);
        return ret;
    }

    mutex_lock(&g_host->m_lock);
    ret = hi_mci_get_hw_lock();
    if (ret) {
        LOG(LOG_ERROR, "get_hard_lock error");
        mmc_release_host(g_host->mmc);
        mutex_unlock(&g_host->m_lock);
        return ret;
    }

    ret = hi_mci_write_protect_set(type, ext_csd, area_index);
    if (ret) {
        LOG(LOG_ERROR, "write protect set error");
    }

    if (ext_csd) {
        kfree(ext_csd);
    }

    mmc_release_host(g_host->mmc);
    hw_release_hard_lock(EMMC_CTLR_LOCK_ID);
    mutex_unlock(&g_host->m_lock);

    return ret;
}
#endif

static const struct mmc_host_ops g_hi_mci_ops = {
    .request = hi_mci_request,
    .set_ios = hi_mci_set_ios,
    .get_ro = hi_mci_get_ro,
    .hw_reset = hi_mci_hw_reset,
#ifdef EMMC_COMPILE
    .execute_tuning = hi_mci_hs200_tuning,
#endif
};

static irqreturn_t hisd_irq(s32 irq, void *dev_id)
{
#ifdef EMMC_COMPILE
    u32 ret;
#endif
    struct himci_host *host = (struct himci_host *)dev_id;
    u32 state;
    u32 int_mask;

    if (!host) {
        LOG(LOG_ERROR, "host is NULL");
        return IRQ_HANDLED;
    }

#ifdef EMMC_COMPILE
    /* The current lock is occupied by another core. A55 does not respond to the data completion interrupt. */
    ret = hw_query_lock_status(EMMC_CTLR_LOCK_ID);
    if (ret != A55_LOCKED) {
        return IRQ_HANDLED;
    }
#endif

    spin_lock(&host->lock);

    /* Obtains the masked interrupt status. */
    state = himci_readl(host->base + HISI_MCI_MINTSTS);

    int_mask = himci_readl(host->base + HISI_MCI_INTMASK);
    himci_writel(0x0, host->base + HISI_MCI_INTMASK);

    /* Clear generated interrupts. */
    himci_writel(state, host->base + HISI_MCI_RINTSTS);

    if (state & DTO_INT_STATUS) {
        host->pending_events |= HIMCI_PEND_DTO_M;
        wake_up(&host->intr_wait);
    }

    /* enable MMC host interrupt */
    himci_writel(int_mask, host->base + HISI_MCI_INTMASK);
    spin_unlock(&host->lock);

    return IRQ_HANDLED;
}

/*
 * Configuring the eMMC/SDIO Frequency Divider of the LP Register
 */
static s32 hi_mci_lp_clk_div_config(void)
{
    s32 ret;
    u_lpsc_crg_div_ctrl lpsc_crg_div_ctrl;

    /*
     * The SDIO frequency is divided by 8.
     * The clock frequency in the AT phase is 400 MHz, and the clock frequency is 25 MHz after frequency division.
     */
    lpsc_crg_div_ctrl.bits.sdio_clk_div_cnt = 0x10;
    /*
     * The frequency of the eMMC is divided by 2, The clock frequency of the AT phase is 400 MHz.
     * The frequency of the eMMC is divided by 4 in the logic.
     * The final frequency after frequency division is 100 MHz.
     */
    lpsc_crg_div_ctrl.bits.emmc_clk_div_cnt = 0x0;

    /* 0x3ff: Clear the eMMC and SDIO frequency division fields and assign values. */
    ret = lp_subsys_reg_read_write(LP_SUBSYS_CSR_LPSC_CRG_DIV_CTRL_OFFSET_ADDR, 0x3ff, lpsc_crg_div_ctrl.val);
    if (ret) {
        LOG(LOG_ERROR, "Config LP clk div cnt failed");
    }

    return ret;
}

static s32 hi_mci_haddr_extend_config(void)
{
    s32 ret;
    u_lpsc_addr_extend0 lpsc_addr_extend0;

#ifdef EMMC_COMPILE
    lpsc_addr_extend0.bits.emmc_addr_remap_mode = EMMC_ADDR_REMAP_MODE;
    lpsc_addr_extend0.bits.emmc_haddr_offset = EMMC_HADDR_OFFSET;
    /* 0x3f: Clears the extended configuration field of the high address of the eMMC and assigns a new value. */
    ret = lp_subsys_reg_read_write(LP_SUBSYS_CSR_LPSC_ADDR_EXTEND0_OFFSET_ADDR, EMMC_EXTEND_ADDR_BIT_MASK,
                                   lpsc_addr_extend0.val);
#else
    lpsc_addr_extend0.bits.sdio_addr_remap_mode = SDIO_ADDR_REMAP_MODE;
    lpsc_addr_extend0.bits.sdio_haddr_offset = SDIO_HADDR_OFFSET;
    ret = lp_subsys_reg_read_write(LP_SUBSYS_CSR_LPSC_ADDR_EXTEND0_OFFSET_ADDR, SDIO_EXTEND_ADDR_BIT_MASK,
                                   lpsc_addr_extend0.val);
#endif
    if (ret) {
        LOG(LOG_ERROR, "Config haddr extend failed");
    }

    return ret;
}

static void hi_mci_soft_reset(void)
{
#ifndef EMMC_COMPILE
    u_lpsc_mdio_sdio_ctrl lp_sdio_ctrl;
#endif

#ifdef EMMC_COMPILE
    /* record hs200 phase config value base on m3 emmc tuning */
    lp_subsys_reg_read(LP_SUBSYS_CSR_LPSC_EMMC_FUNC_CTRL_OFFSET_ADDR, &g_lpsc_emmc_ctrl_reg);
    m_emmc_reset(LP_SUBSYS_CSR_LPSC_EMMC_CRG_CTRL_OFFSET_ADDR, LP_EMMC_MASK);
    mmc_phase_config(MMC_COMPATIBILITY_MODE, 0);
#else
    if (m_reset_handle(LP_SUBSYS_CSR_LPSC_SRST_REQ0_OFFSET_ADDR, SDIO_SRST_MASK)) {
        LOG(LOG_ERROR, "sdio reset fail");
    }

    lp_subsys_reg_read(LP_SUBSYS_CSR_LPSC_MDIO_SDIO_CTRL_OFFSET_ADDR, &lp_sdio_ctrl.val);
    /* When the SDIO is set to 25 Mbit/s, set this bit to 1. When the SDIO is set to 50 Mbit/s, set this bit to 0. */
    lp_sdio_ctrl.bits.mmc_cclk_samp_ctrl = 0x1;
    lp_subsys_reg_write(LP_SUBSYS_CSR_LPSC_MDIO_SDIO_CTRL_OFFSET_ADDR, lp_sdio_ctrl.val);
#endif

    return;
}

static void hi_mci_mmc_init(struct mmc_host *mmc)
{
    mmc->ops = &g_hi_mci_ops;
    mmc->f_min = MMC_CCLK_MIN;

    /* The default value is 4-bit. The eMMC supports 8-bit. */
    mmc->caps |= MMC_CAPS_MASK;
#ifdef EMMC_COMPILE
    mmc->caps |= MMC_CAP_8_BIT_DATA;
#if LINUX_VERSION_CODE < KERNEL_VERSION(5, 10, 0)
    mmc->caps2 |= MMC_CAP2_HS200_1_8V_SDR;
#else
    mmc->caps2 |= MMC_CAP2_HS200_1_8V_SDR | MMC_CAP2_NO_SDIO | MMC_CAP2_NO_SD;
#endif
    mmc->f_max = EMMC_CCLK_MAX;
#else
    mmc->f_max = SDIO_CCLK_MAX;
#endif

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 37))
    mmc->max_segs = MMC_MAX_SEGMENTS;
#else
    mmc->max_hw_segs = MMC_MAX_HW_SEGMENTS;
    mmc->max_phys_segs = MMC_MAX_PHYS_SEGMENTS;
#endif
    /* reload by this controller */
    mmc->max_blk_size = MMC_MAX_BLOCK_SIZE;
    mmc->max_blk_count = MMC_MAX_BLOCK_COUNT;
    mmc->max_req_size = mmc->max_blk_count * mmc->max_blk_size;
    mmc->ocr_avail = MMC_VDD_32_33 | MMC_VDD_33_34;

    return;
}

static s32 hi_mci_irq_init(struct platform_device *pdev, struct himci_host *host)
{
    s32 ret;
    s32 irq;

    irq = platform_get_irq(pdev, 0);
    if (irq < 0) {
        LOG(LOG_ERROR, "no IRQ defined");
        return -EFAULT;
    }

    init_waitqueue_head(&host->intr_wait);

    ret = request_irq((u32)irq, hisd_irq, 0, pdev->name, host);
    if (ret) {
        LOG(LOG_ERROR, "request_irq error");
        return ret;
    }
    host->irq = (u32)irq;

    return 0;
}

static s32 hi_mci_host_init(struct platform_device *pdev, struct himci_host *host,
                            struct mmc_host *mmc)
{
    s32 ret;
    struct resource *res = NULL;
    u64 dma_bit_mask = MMC_DMA_BIT_MAST;

    host->dma_vaddr = (u32 *)dma_alloc_coherent(&pdev->dev, MMC_MAX_PAGE_COUNT * PAGE_SIZE,
                                                &host->dma_paddr,
                                                GFP_KERNEL);
    if (!host->dma_vaddr) {
        LOG(LOG_ERROR, "no mem for himci dma");
        return -ENOMEM;
    }

    host->mmc = mmc;
    host->irq = -1;

    res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
    if (!res) {
        LOG(LOG_ERROR, "himci get resource failed");
        return -ENOMEM;
    }

    host->base = ioremap(res->start, resource_size(res));
    if (!host->base) {
        LOG(LOG_ERROR, "no mem for himci base");
        return -ENOMEM;
    }

    mutex_init(&host->m_lock);

    /* Sets the internal DMA support bus address bit 34 */
    dma_set_mask_and_coherent(&pdev->dev, DMA_BIT_MASK(dma_bit_mask));

    /* Configuring the eMMC Controller */
    spin_lock_init(&host->lock);

    hi_mci_init_card(host);

    host->card_status = hi_mci_sys_card_detect(host);

    ret = hi_mci_irq_init(pdev, host);
    if (ret) {
        LOG(LOG_ERROR, "irq init error");
        return ret;
    }

    g_host = host;
    return 0;
}

static void hi_mci_timer_init(struct himci_host *host)
{
#ifndef EMMC_COMPILE
    timer_setup(&host->timer, hi_mci_detect_card, 0);
    host->timer.expires = jiffies + HI_MCI_DETECT_TIMEOUT;
    add_timer(&host->timer);
#endif

    return;
}

static void hi_mci_init_err_handle(struct platform_device *pdev, struct himci_host *host,
                                   struct mmc_host *mmc)
{
    if (host) {
        if (host->base) {
            iounmap(host->base);
        }

        if (host->dma_vaddr) {
            dma_free_coherent(&pdev->dev, MMC_MAX_PAGE_COUNT * PAGE_SIZE,
                              host->dma_vaddr, host->dma_paddr);
        }

        if (host->irq >= 0) {
            free_irq(host->irq, host);
        }
    }

    if (mmc) {
        mmc_free_host(mmc);
    }

    return;
}

static s32 hi_mci_probe(struct platform_device *pdev)
{
    struct mmc_host *mmc = NULL;
    struct himci_host *host = NULL;
    s32 ret;

    g_write_all_time = jiffies;

    /* Soft reset controller and configuration phase */
    hi_mci_soft_reset();

    /* Configuring the LP Frequency Divider */
    hi_mci_lp_clk_div_config();

    /* Configuring Address Extension */
    hi_mci_haddr_extend_config();

    mmc = mmc_alloc_host(sizeof(struct himci_host), &pdev->dev);
    if (!mmc) {
        LOG(LOG_ERROR, "no mem for hi mci host controller");
        ret = -ENOMEM;
        goto out;
    }

    hi_mci_mmc_init(mmc);

    host = (struct himci_host *)mmc_priv(mmc);
    ret = hi_mci_host_init(pdev, host, mmc);
    if (ret) {
        LOG(LOG_ERROR, "himci host init failed");
        goto out;
    }

    ret = mmc_add_host(mmc);
    if (ret) {
        LOG(LOG_ERROR, "cannot add mmc host");
        goto out;
    }
    platform_set_drvdata(pdev, mmc);

    hi_mci_timer_init(host);

    return 0;
out:
    hi_mci_init_err_handle(pdev, host, mmc);
    return ret;
}

static s32 hi_mci_remove(struct platform_device *pdev)
{
    struct mmc_host *mmc = NULL;
    struct himci_host *host = NULL;

    mmc = (struct mmc_host *)platform_get_drvdata(pdev);
    if (mmc) {
        mmc_remove_host(mmc);
        host = (struct himci_host *)mmc_priv(mmc);
        if (host) {
            hi_mci_ctrl_power(host, POWER_OFF);
            hi_mci_control_cclk(host, DISABLE);
            iounmap(host->base);
            dma_free_coherent(&pdev->dev, MMC_MAX_PAGE_COUNT * PAGE_SIZE,
                              host->dma_vaddr, host->dma_paddr);
#ifndef EMMC_COMPILE
            del_timer_sync(&host->timer);
#endif
            free_irq(host->irq, host);
        }
        mmc_free_host(mmc);
    }

    platform_set_drvdata(pdev, NULL);

    g_host = NULL;
    return 0;
}

static void hi_mci_shutdown(struct platform_device *pdev)
{
#ifdef EMMC_COMPILE
    if (hw_query_lock_status(EMMC_CTLR_LOCK_ID) == A55_LOCKED) {
        hw_release_hard_lock(EMMC_CTLR_LOCK_ID);
    }
#endif

    return;
}

#ifdef CONFIG_PM
static s32 hi_mci_suspend(struct platform_device *dev, pm_message_t state)
{
    struct mmc_host *mmc = (struct mmc_host *)platform_get_drvdata(dev);
    struct himci_host *host = NULL;
    s32 ret = 0;

    if (mmc) {
        host = (struct himci_host *)mmc_priv(mmc);
        del_timer_sync(&host->timer);
    }

    return ret;
}

static s32 hi_mci_resume(struct platform_device *dev)
{
    struct mmc_host *mmc = (struct mmc_host *)platform_get_drvdata(dev);
    struct himci_host *host = NULL;
    s32 ret = 0;

    if (mmc) {
        host = (struct himci_host *)mmc_priv(mmc);
        /* enable mmc clk */
        /* enable card */
        hi_mci_init_card(host);
        add_timer(&host->timer);
    }

    return ret;
}
#endif

#ifdef EMMC_COMPILE
static s32 mmc_ioctl_get_reg(unsigned long arg)
{
    s32 ret;
    mmc_reg_info_s mmc_opt_s = { 0 };

    ret = copy_from_user(&mmc_opt_s, (void __user *)arg, sizeof(mmc_reg_info_s));
    if (ret) {
        LOG(LOG_ERROR, "copy from user failed");
        return -EAGAIN;
    }

    ret = hi_mci_mmc_reg_get(mmc_opt_s.mmc_reg_type_v, mmc_opt_s.mmc_reg_offset, mmc_opt_s.length, mmc_opt_s.buf);
    if (ret) {
        LOG(LOG_ERROR, "mmc get register failed");
        return ret;
    }

    return 0;
}

static s32 mmc_ioctl_get_hynix_health_report(unsigned long arg)
{
    s32 ret;
    u8 *ext_csd = NULL;

    mmc_claim_host(g_host->mmc);
    ret = hi_mci_cmd_send(HYNIX_CMD60_ARG1, MMC_CMD_AC, CMD60, 0);
    if (ret) {
        LOG(LOG_ERROR, "mmc cmd60_1 execute failed");
        mmc_release_host(g_host->mmc);
        return ret;
    }

    mdelay(100); /* delay 100ms */
    ret = hi_mci_cmd_send(HYNIX_CMD60_ARG2, MMC_CMD_AC, CMD60, 0);
    if (ret) {
        LOG(LOG_ERROR, "mmc cmd60_2 execute failed");
        mmc_release_host(g_host->mmc);
        return ret;
    }

    ret = mmc_get_ext_csd(g_host->mmc->card, &ext_csd);
    mmc_release_host(g_host->mmc);
    if (ret) {
        LOG(LOG_ERROR, "send cmd for ext_csd reg failed");
        return ret;
    }

    ret = copy_to_user((void __user *)arg, ext_csd, EXT_CSD_COUNT);
    if (ret) {
        LOG(LOG_ERROR, "copy to user failed");
        kfree(ext_csd);
        return -EFAULT;
    }

    kfree(ext_csd);

    return 0;
}

static s32 mmc_ioctl_get_tbw(unsigned long arg)
{
    s32 ret;
    mmc_write_allinfo_s mmc_write_allinfo_s = { 0 };

    ret = copy_from_user(&mmc_write_allinfo_s, (void __user *)arg, sizeof(mmc_write_allinfo_s));
    if (ret) {
        LOG(LOG_ERROR, "copy from user failed");
        return -EAGAIN;
    }

    mmc_write_allinfo_s.write_byte = g_write_all_byte;
    mmc_write_allinfo_s.write_time = (jiffies - g_write_all_time) / HZ;

    if (mmc_write_allinfo_s.mmc_clear_flag == MMC_CLEAR) {
        g_write_all_byte = 0;
        g_write_all_time = jiffies;
    }

    ret = copy_to_user((void __user *)arg, &mmc_write_allinfo_s, sizeof(mmc_write_allinfo_s));
    if (ret) {
        LOG(LOG_ERROR, "copy to user failed");
        return -EFAULT;
    }

    return 0;
}

static s32 mmc_ioctl_update_protect(unsigned long arg)
{
    s32 ret;
    mmc_area_protect_s area_protect = { 0 };

    ret = copy_from_user(&area_protect, (void __user *)arg, sizeof(mmc_area_protect_s));
    if (ret) {
        LOG(LOG_ERROR, "mmc copy from user failed");
        return -EAGAIN;
    }

    return hi_mci_gpp_write_protect(area_protect.type, area_protect.area_index);
}

static s32 mmc_ioctl_erase(unsigned long arg)
{
    struct mmc_do_erase_info erase_info = { 0 };
    struct mmc_card *card = g_host->mmc->card;
    u8 *ext_csd = NULL;
    u8 cur_area;
    s32 ret;

    ret = copy_from_user(&erase_info, (void __user*)arg, sizeof(struct mmc_do_erase_info));
    if (ret) {
        LOG(LOG_ERROR, "copy from user fail");
        return -EAGAIN;
    }

    LOG(LOG_PRINT, "erase- addr:0x%x blk_num:%u arg:%u card_type:%u card_erasesiz:%u",
        erase_info.start_addr_arg, erase_info.blk_num, erase_info.erase_arg,
        card->type, card->erase_size);

    if (!mmc_can_erase(g_host->mmc->card)) {
         LOG(LOG_ERROR, "mmc can't erase");
         return -EAGAIN;
    }

    mmc_claim_host(g_host->mmc);

    ret = mmc_get_ext_csd(card, &ext_csd);
    if (ret) {
        LOG(LOG_ERROR, "get ext csd failed");
        mmc_release_host(g_host->mmc);
        return ret;
    }

    cur_area = ext_csd[EXT_CSD_PART_CONFIG];

    /* Switching from the area to the user area */
    ret = mmc_switch(card, EXT_CSD_CMD_SET_NORMAL, EXT_CSD_PART_CONFIG, 0,
                     card->ext_csd.part_time);
    if (ret) {
        LOG(LOG_ERROR, "mmc switch failed");
        goto exit;
    }

    ret = mmc_erase(g_host->mmc->card, erase_info.start_addr_arg, erase_info.blk_num,
                    erase_info.erase_arg);
    if (ret) {
        LOG(LOG_ERROR, "mmc erase failed");
        goto exit;
    }

    /* Revert to the area before erasing */
    ret = mmc_switch(card, EXT_CSD_CMD_SET_NORMAL, EXT_CSD_PART_CONFIG, cur_area,
                     card->ext_csd.part_time);

exit:
    kfree(ext_csd);
    mmc_release_host(g_host->mmc);

    return ret;
}

static long mmc_ioctl(struct file *pfile, u32 cmd, unsigned long arg)
{
    s32 ret;

    switch (cmd) {
        case MMC_GET_REG:
            ret = mmc_ioctl_get_reg(arg);
            break;

        case MMC_GET_HYNIX_HEALTH_REPORT:
            ret = mmc_ioctl_get_hynix_health_report(arg);
            break;

        case MMC_GET_TBW:
            ret = mmc_ioctl_get_tbw(arg);
            break;

        case MMC_UPDATE_PROTECT:
            ret = mmc_ioctl_update_protect(arg);
            break;

        case MMC_ERASE_DATA:
            ret = mmc_ioctl_erase(arg);
            break;

        default:
            LOG(LOG_ERROR, "cmd invalid fail cmd = 0x%x", _IOC_NR(cmd));
            ret = -EFAULT;
    }

    return ret;
}
#endif

#ifdef EMMC_COMPILE

const struct file_operations g_mmc_fops = {
    .owner = THIS_MODULE,
    .unlocked_ioctl = mmc_ioctl
};
#else

const struct file_operations g_sdio_fops = {
    .owner = THIS_MODULE,
    .unlocked_ioctl = NULL,
};
#endif
static s32 hi_mci_cdev_probe(struct platform_device *pdev)
{
    s32 ret;

    g_mmc_dev = (cdev_st *)kmalloc(sizeof(cdev_st), GFP_KERNEL);
    if (g_mmc_dev == NULL) {
        LOG(LOG_ERROR, "g_mmc_dev err!");
        return -ENOMEM;
    }
    (void)memset_s(g_mmc_dev, sizeof(cdev_st), 0, sizeof(cdev_st));

#ifdef EMMC_COMPILE
    ret = comm_init_dev(g_mmc_dev, &g_mmc_fops, DEV_MMC_NAME);
#else
    ret = comm_init_dev(g_mmc_dev, &g_sdio_fops, DEV_SDIO_NAME);
#endif
    if (ret) {
        goto EXIT;
    }

    ret = hi_mci_probe(pdev);
    if (ret) {
        comm_cleanup_dev(g_mmc_dev);
        LOG(LOG_ERROR, "hicmi probe err!");
        ret = -ENOMEM;
        goto EXIT;
    }

EXIT:
    if (ret) {
        kfree(g_mmc_dev);
        g_mmc_dev = NULL;
        LOG(LOG_ERROR, "comm_init_dev err");
    }

    return ret;
}

static s32 hi_mci_cdev_remove(struct platform_device *pdev)
{
    if (g_mmc_dev != NULL) {
        comm_cleanup_dev(g_mmc_dev);
        kfree(g_mmc_dev);
        g_mmc_dev = NULL;
    }

    return hi_mci_remove(pdev);
}

#ifdef EMMC_COMPILE
static const struct of_device_id g_hi_mci_of_match[] = {
    { .compatible = "hisilicon,hi1711-emmc" }, {}
};
#else
static const struct of_device_id g_hi_mci_of_match[] = {
    { .compatible = "hisilicon,hi1711-sdio" }, {}
};
#endif

static struct platform_driver g_hi_mci_driver = {
    .probe = hi_mci_cdev_probe,
    .remove = hi_mci_cdev_remove,
    .shutdown = hi_mci_shutdown,
    .suspend = hi_mci_suspend,
    .resume = hi_mci_resume,
    .driver = {
#ifdef EMMC_COMPILE
    .name = "hi_emmc",
#else
    .name = "hi_sdio",
#endif
    .of_match_table = g_hi_mci_of_match,
    },
};

module_platform_driver(g_hi_mci_driver);

MODULE_AUTHOR("HiSilicon Technologies Co., Ltd");
MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("MMC/SD driver for the Hisilicon MMC/SD Host Controller");