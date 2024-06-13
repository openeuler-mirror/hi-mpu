/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 */

#include "bm_common.h"
#include "bm_config.h"
#include "bm_hwi.h"
#include "bm_message.h"
#include "sample_common.h"
#include "bm_uart.h"

XFER_DATA unsigned char g_sdata[] = {0x12, 0x34, 0x12, 0x34};  // buf 0x12, 0x34, 0x12, 0x34
XFER_DATA unsigned char g_send_buf[] = {0x32, 0x14, 0x53, 0x65};
XFER_DATA unsigned char g_recv_buf[4];  // bufsize 4

static void print_data(unsigned char *buf, int size)
{
    for (int i = 0; i < size; i++) {
        bm_printf("%.2x ", buf[i]);
    }
    bm_printf("\n");
}

static void core3_proc_ipi_recv(void *arg)
{
    (void)arg;
    unsigned int msg_id = 0;
    unsigned char *str = (unsigned char *)bm_get_ipi_msg();
    print_data(str, 4); // 4 byte
    unsigned char ss[4]; // bufsize 15
    (void)bm_recv_msg_with_ipi(&msg_id, ss, sizeof(ss));
    bm_log("msgid = %d\n", msg_id);
    print_data(ss, sizeof(ss));
    bm_msg_show_perf();
}

static void core3_proc_send(void)
{
    bm_request_irq(IPI_INT_NO, core3_proc_ipi_recv, NULL);
    bm_enable_irq(IPI_INT_NO);
    (void)bm_send_msg(MSG_ID_0, g_sdata, sizeof(g_sdata));
}

static void core0_proc_recv(void)
{
    int target_core = 3; // core 3
    unsigned char *str = (unsigned char *)bm_get_msg(MSG_ID_0);
    print_data(str, 4); // 4 byte

    (void)bm_recv_msg(MSG_ID_0, g_recv_buf, sizeof(g_recv_buf));
    print_data(g_recv_buf, sizeof(g_recv_buf));
    bm_enable_irq(IPI_INT_NO);
    (void)bm_send_msg_with_ipi(target_core, MSG_ID_1, g_send_buf, sizeof(g_send_buf));
}

/* ************ sample_msg start ************ */
int main(void)
{
    sample_prepare();
    unsigned int msg_core = bm_get_coreid();
    switch (msg_core) {
        case SAMPLE_CORE0:
            core0_proc_recv();
            return BM_OK;
        case SAMPLE_CORE1:
            break;
        case SAMPLE_CORE2:
            break;
        case SAMPLE_CORE3:
            core3_proc_send();
            break;
        default:
            bm_log("sample_msg: invalid core_num\n");
            break;
    }
    while (1) {
    }
    return BM_OK;
}
/* ************ sample_msg end ************ */