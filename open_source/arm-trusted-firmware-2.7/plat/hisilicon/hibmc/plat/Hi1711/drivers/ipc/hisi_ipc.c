/* Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: hi1711_ipc
 * Author: xxx
 * Create: 2022-11-16
 */
#include <arch_helpers.h>
#include <lib/mmio.h>
#include <plat/common/platform.h>
#include <hisi_ipc.h>
#include <../hi1711_private.h>

/*********************************************************
 * bit[31:24]: 发起对象ID--AP:1
 * bit[23:16]: 操作对象ID--AP:1
 * bit[15:8] : 命令模式(上电-下电):0x00--ON, 0x01--OFF
 * bit[7:0]  : 命令类型0x01-电源
 *********************************************************/
#define IPC_CMD_TYPE(src_obj, oper_obj, cmd_mode, cmd_type) \
    (((src_obj) << 24) | ((oper_obj) << 16) | ((cmd_mode) << 8) | (cmd_type))

/*********************************************************
 * bit[15:8]:0~no idle, 1~idle
 * bit[7:0]:cpux
 *********************************************************/
#define IPC_CMD_PARA(src_obj, cpu) \
    (((src_obj) << 8) | (cpu))

/* 状态机状态；
4'b0001 空闲状态，邮箱空闲；
4'b0010 源状态，此状态下，邮箱通道已占用，还未发中断给目的处理器；
4'b0100 目的状态，此状态下，目的处理器收到源发中断，未全部清除或返回应答中断。
4'b1000 应答状态，此状态下，源收到应答中断未清除完毕。
其他：异常值
 */
#define IPC_STATE_IDLE 0x10

enum src_id {
    OBJ_AP = 1,      /* 0 */
    OBJ_LIT_CLUSTER, /* 1 */
    OBJ_BIG_CLUSTER, /* 2 */
    OBJ_GPU,         /* 3 */
    OBJ_DDR,         /* 4 */
    OBJ_ASP,         /* 5 */
    OBJ_HIFI,        /* 6 */
    OBJ_IOMCU,       /* 7 */
    OBJ_LPM3,        /* 8 */
    OBJ_MODEM,       /* 9 */
    OBJ_SYS,         /* 10 */
    OBJ_HKADC,       /* 11 */
    OBJ_REGULATOR,   /* 12 */
    OBJ_CLK,         /* 13 */
    OBJ_TEMPERTURE,  /* 14 */
    OBJ_PMU,         /* 15 */
    OBJ_PSCI,        /* 16 */
    OBJ_TELEMNTN,    /* 17 */
    OBJ_MCA,         /* 18 */
    OBJ_INSE,        /* 19 */
    OBJ_TEST,        /* 20 */
    OBJ_TEEOS,       /* 21 */
#ifdef CONFIG_HISI_FCM_CLUSTER
    OBJ_MID_CLUSTER, /* 22 */
    OBJ_L3_CLUSTER,  /* 23 */
#endif
    MAX_CMD_OBJ /* 24 */
};

/* lpm3's mailboxs are 13~17 */
enum lpm3_mbox_id {
    LPM3_MBX0 = 0,
    LPM3_MBX1,
    LPM3_MBX2,
    LPM3_MBX3,
    LPM3_MBX4,
};

enum cmd_mode {
    CMD_ON = 0x0,   /* 0 */
    CMD_OFF,        /* 1 */
    CMD_INQUIRY,    /* 2 */
    CMD_SETTING,    /* 3 */
    CMD_NOTIFY,     /* 4 */
    CMD_TEST,       /* 5 */
    CMD_NOBUSINESS, /* 6 */
    MAX_CMD_MODE    /* 7 */
};

enum cmd_type {
    TYPE_RESERVED = 0x0, /* 0 */
    TYPE_POWER,          /* 1 */
    TYPE_CLK,            /* 2 */
    TYPE_CORE,           /* 3 */
    TYPE_CLUSTER,        /* 4 */
    TYPE_SLEEP,          /* 5 */
    TYPE_SR,             /* 6 */
    TYPE_MODE,           /* 7 */
    TYPE_UPLIMIT,        /* 8 */
    TYPE_DNLIMIT,        /* 9 */
    TYPE_FREQ,           /* 10 */
    TYPE_T,              /* 11 */
    TYPE_VOLT,           /* 12 */
    TYPE_RESET,          /* 13 */
    TYPE_PWC,            /* 14 */
    TYPE_TEST,           /* 15 */
    TYPE_BIST_ON,        /* 16 */
    TYPE_BIST_CMD,       /* 17 */
    TYPE_BIST_ALGO,      /* 18 */
    TYPE_BIST_OFF,       /* 19 */
    TYPE_CURRENT,        /* 20 */
    TYPE_DDR_TIME,       /* 21 */
    TYPE_DDR_FLUX,       /* 22 */
    TYPE_READ,           /* 23 */
    TYPE_WRITE,          /* 24 */

    /* reserve */
    /* 10 cmd_types are used for InSE only */
    TYPE_INSE_A = 0x50, /* 0x50 */
    TYPE_INSE_B,        /* 0x51 */
    TYPE_INSE_C,        /* 0x52 */
    TYPE_INSE_D,        /* 0x53 */
    TYPE_INSE_E,        /* 0x54 */
    TYPE_INSE_F,        /* 0x55 */
    TYPE_INSE_G,        /* 0x56 */
    TYPE_INSE_H,        /* 0x57 */
    TYPE_INSE_I,        /* 0x58 */
    TYPE_INSE_TEST,     /* 0x59 */
    /* reserve */
    MAX_CMD_TYPE /* 0x5A */
};

static void cpu_relax(void)
{
    volatile int i;

    for (i = 0; i < 10; i++) { /* loop 10 */
        nop();
    }
}

static inline void hisi_ipc_clear_ack(enum src_id source,
                                      enum lpm3_mbox_id mbox)
{
    unsigned int int_status;

    do {
        int_status = mmio_read_32(IPC_MBX_MODE_REG(mbox));
        int_status &= 0xF0;
        cpu_relax();
    } while (int_status != IPC_ACK_BIT_SHIFT); /* 应答状态，此状态下，源收到应答中断未清除完毕 */

    mmio_write_32(IPC_MBX_ICLR_REG(mbox), source);
}

static void hisi_ipc_send_cmd_with_ack(enum src_id source, enum lpm3_mbox_id mbox,
                                       unsigned int cmdtype, unsigned int cmdpara)
{
    unsigned int mask = 0;
    unsigned int state;
    unsigned int value;

    mmio_write_32(IPC_LOCK_REG, IPC_UNLOCK_VALUE);

    /* wait for idle and occupy */
    while (1) {
        /* 1. 读取mailbox状态. */
        state = mmio_read_32(IPC_MBX_MODE_REG(mbox));
        if ((state & 0x10) != IPC_STATE_IDLE) {
            cpu_relax();
        } else {
            /* 2. 测试邮箱读写是否正常. */
            mmio_write_32(IPC_MBX_SOURCE_REG(mbox), source);
            value = mmio_read_32(IPC_MBX_SOURCE_REG(mbox));
            if (source == value) {
                break;
            }
        }
    }

    /* 3.设置Mailbox自动应答使能.
    自动应答，即当目的处理器中断被清除0后,
    mailbox自动发出一个回复中断给源处理器.
    0：禁止;
    1：使能. */
    mmio_write_32(IPC_MBX_MODE_REG(mbox), 0x1);
    __asm__ volatile("dsb sy");

    /* mask the other cpus */
    /*
    邮箱通道到处理器的中断屏蔽设置，每一bit对应一个处理器。
    0：不屏蔽;
    1：屏蔽.
    位宽与宏定义IPC_CPU_NUM一致。(SDRV200为3)
    int_mask: 0-2. */
    mmio_write_32(IPC_MBX_IMASK_REG(mbox), mask);

    /* set data */
    mmio_write_32(IPC_MBX_DATA_REG(mbox, 0), cmdtype);

    mmio_write_32(IPC_MBX_DATA_REG(mbox, 1), cmdpara);
    /* send cmd */
    /* 中断发送寄存器，每一bit对应一个处理器。
    通信过程中，配置此寄存器，只置位bit有效。
    当源发中断给目的时，send寄存器配置值与源寄存器值相等时
    发中断给目的处理器；当目的发应答中断给源时，
    send寄存器配置值与目的相等时发应答中断给源。
    自动链接时，被链接邮箱此寄存器不须配置；
    自动应答时，目的清除中断后，此寄存器自动置位，不须配置 */
    mmio_write_32(IPC_MBX_SEND_REG(mbox), source);
    __asm__ volatile("dsb sy");

    /* wait ack and clear */
    hisi_ipc_clear_ack(source, mbox);
    __asm__ volatile("dsb sy");

    /* release mailbox */
    mmio_write_32(IPC_MBX_SOURCE_REG(mbox), source);

    return;
}

void hisi_ipc_pm_on_off(unsigned int core,
                        unsigned int cluster,
                        enum pm_mode mode)
{
    unsigned int cmdtype;
    unsigned int cmdpara;
    enum src_id source = OBJ_AP;
    enum lpm3_mbox_id mailbox = LPM3_MBX3; /* mailbox2-->mailbox3 */

    cmdtype = IPC_CMD_TYPE(OBJ_AP, OBJ_AP, mode, TYPE_POWER);
    cmdpara = IPC_CMD_PARA(OBJ_AP, core);

    hisi_ipc_send_cmd_with_ack(source, mailbox, cmdtype, cmdpara);

    return;
}
