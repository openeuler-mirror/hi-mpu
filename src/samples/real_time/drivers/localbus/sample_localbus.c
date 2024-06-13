/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 */
#include "bm_common.h"
#include "bm_localbus.h"
#include "sample_common.h"

#define LOCALBUS_SIZE_MAX 64
struct localbus_ops g_ops;
struct localbus_cs_ops g_cs_ops;

XFER_DATA unsigned char g_tmpbuf[LOCALBUS_SIZE_MAX];

static int cpld_get_data(struct localbus_cs_ops *cs_ops, long long from, unsigned int length, unsigned char *buffer)
{
    unsigned int i = 0;
    unsigned char *l_puc = NULL;
    unsigned short *l_pus = NULL;
    unsigned int addr = cs_ops->csi->start + from;

    bm_log("cs_ops->csi->width = %x\n", cs_ops->csi->width);
    /* 8位位宽访问 */
    if (cs_ops->csi->width == LOCALBUS_WIDTH_8) {
        l_puc = (unsigned char *)buffer;
        while (i < length) {
            *l_puc = *(volatile unsigned char *)((uintptr_t)addr);
            i += sizeof(unsigned char);
            addr += sizeof(unsigned char);
            l_puc++;
        }
    } else if (cs_ops->csi->width == LOCALBUS_WIDTH_16) {
        /* 16位位宽访问 */
        l_pus = (unsigned short *)buffer;
        while (i < length) {
            *l_pus = *(volatile unsigned short *)((uintptr_t)addr);
            i += sizeof(unsigned short);
            addr += sizeof(unsigned short);
            l_pus++;
        }
    } else {
        return BM_FAIL;
    }
    return 0;
}

static int cpld_write_data(struct localbus_cs_ops *cs_ops, long long to, unsigned int length, unsigned char *buffer)
{
    unsigned int i = 0;
    unsigned char *l_puc = NULL;
    unsigned short *l_pus = NULL;
    unsigned int addr = cs_ops->csi->start + to;

    bm_log("cs_ops->csi->width = %x\n", cs_ops->csi->width);
    /* 8位位宽访问 */
    if (cs_ops->csi->width == LOCALBUS_WIDTH_8) {
        l_puc = (unsigned char *)buffer;
        while (i < length) {
            *(volatile unsigned char *)((uintptr_t)(addr)) = (unsigned char)(*l_puc);
            i += sizeof(unsigned char);
            addr += sizeof(unsigned char);
            l_puc++;
        }
    } else if (cs_ops->csi->width == LOCALBUS_WIDTH_16) {
        /* 16位位宽访问 */
        l_pus = (unsigned short *)buffer;
        while (i < length) {
            *(volatile unsigned char *)((uintptr_t)(addr)) = (unsigned char)(*l_pus);
            i += sizeof(unsigned short);
            addr += sizeof(unsigned short);
            l_pus++;
        }
    } else {
        return BM_FAIL;
    }
    return BM_OK;
}

static void cpld_destroy(struct localbus_cs_ops *cs_ops)
{
    bm_log("cs%d destroy....\n", cs_ops->cs);
}

static int cpld_probe(struct localbus_cs_ops *cs_ops)
{
    bm_log("cs%d probe....\n", cs_ops->cs);
    return 0;
}

static int localbus_cmd_rw_cpld(void)
{
    localbus_init();
    g_ops.probe = cpld_probe;
    g_ops.destroy = cpld_destroy;
    g_ops.read = cpld_get_data;
    g_ops.write = cpld_write_data;
    g_cs_ops.ops = &g_ops;
    g_cs_ops.cs = 1;
    localbus_ops_register(&g_cs_ops, LOCALBUS_WIDTH_8);

    g_tmpbuf[0] = 0xff;
    localbus_read(1, 0x0e, 1, g_tmpbuf);
    bm_log("read 0x4000_000e = %x\n", g_tmpbuf[0]);

    g_tmpbuf[0] = 0x5a;
    localbus_write(1, 0x0e, 1, g_tmpbuf);

    g_tmpbuf[0] = 0x00;
    localbus_read(1, 0x0e, 1, g_tmpbuf);
    bm_log("read 0x4000_000e = %x\n", g_tmpbuf[0]);

    localbus_ops_unregister(&g_cs_ops);
    g_tmpbuf[0] = 0xff;
    localbus_read(1, 0x0e, 1, g_tmpbuf);

    localbus_erase(1, 0x0e, 0x100);
    localbus_destroy();
    return 0;
}

static void sample_localbus(unsigned int core)
{
    (void)core;
    localbus_cmd_rw_cpld();
}

/* ************ sample_localbus start ************ */
#if defined(__BAREMETAL__)
int main(void)
#elif defined(__LIBCK__)
int app_main(void)
#endif
{
    sample_prepare();
    unsigned int localbus_core = bm_get_coreid();
    switch (localbus_core) {
        case SAMPLE_CORE0:
            return BM_OK;
        case SAMPLE_CORE1:
            break;
        case SAMPLE_CORE2:
            break;
        case SAMPLE_CORE3:
            sample_localbus(localbus_core);
            break;
        default:
            bm_log("sample_localbus: invalid core_num\n");
            break;
    }
    while (1) {
    }
    return BM_OK;
}
/* ************ sample_localbus end ************ */