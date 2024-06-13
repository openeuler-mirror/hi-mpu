/* Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: hi1711_nmi
 * Author: xxx
 * Create: 2022-11-16
 */
#include <arch_helpers.h>
#include <common/runtime_svc.h>
#include <common/debug.h>
#include <string.h>
#include <lib/el3_runtime/context_mgmt.h>
#include <plat/common/platform.h>
#include <drivers/arm/gicv3.h>
#include "../../hi1711_def.h"

/* 命令字 */
#define PRIVATE_NMI_SMC_FIQ_GLUE_ID          0x83000004
#define PRIVATE_NMI_SMC_FIQ_GLUE_RETURN_CODE 0x83000005

#define NMI_STACK_SIZE 0x200

uint64_t nmi_RTOS_context_addr __attribute__((section("tzfw_coherent_mem")));
uint64_t nmi_transfer_add __attribute__((section("tzfw_coherent_mem")));

uint64_t checksum(uint8_t *buf, uint64_t size)
{
    uint64_t i;
    uint64_t sum = 0;

    for (i = 0; i < size; i++) {
        sum += (uint64_t) * (buf + i);
    }

    return sum;
}

uint64_t read_sp_el1(void)
{
    uint64_t val = 0;

    __asm__ volatile("mrs %0, sp_el1" : "=r"(val));

    return val;
}

void SaveContext(void *handle, uint32_t linear_id, uint64_t id)
{
    uint32_t i;
    uint32_t cpu_no = linear_id;
    uint64_t *addr_stack = (uint64_t *)(nmi_RTOS_context_addr + cpu_no * NMI_STACK_SIZE);
    volatile uint64_t *addr = addr_stack;
    uint64_t tmp;

    INFO("Save context for RTOS...\n");

    for (i = 0; i < GP_NUM; i++) { /* RTOS 数据定义寄存器只有31个 */
        tmp = read_ctx_reg(get_gpregs_ctx(handle), (CTX_GPREG_X0 + 8 * i)); /* 8*i */
        *(addr++) = (tmp);
    }

    tmp = read_sp_el1();
    (*(addr++)) = (tmp);
    tmp = read_ctx_reg(get_el3state_ctx(handle), CTX_ELR_EL3);
    (*(addr++)) = (tmp);
    tmp = read_ctx_reg(get_el3state_ctx(handle), CTX_SPSR_EL3);
    (*(addr++)) = (tmp);
    /* orig_x0 */
    (*(addr++)) = (tmp);
    /* syscallno */
    (*(addr++)) = (tmp);
    tmp = PERI_WDT5_S;
    (*(addr++)) = (tmp);

    /* OS下定义的结构体大小为288个字节，这只算288字节的sum */
    tmp = checksum((uint8_t *)addr_stack, 288);
    (*(addr++)) = (tmp);

    return;
}

void nmi_trigger_sgi_to_cores(int sgi_num)
{
    uint64_t sgi_val;

    /* Raise SGI to PE specified by its affinity */
    sgi_val = GICV3_SGIR_VALUE(0, 0, 0, sgi_num, 1, 0);

    /*
     * Ensure that any shared variable updates depending on out of band
     * interrupt trigger are observed before raising SGI.
     */
    dsbishst();
    write_icc_sgi0r_el1(sgi_val);
    isb();
}

uint64_t ext_int0_int_handler(uint32_t id, uint32_t flags, void *handle, void *cookie)
{
    uint32_t linear_id = plat_my_core_pos();
    uint32_t spsr_el3;

    if (id == PERI_WDT5_S) {
        gicv3_disable_interrupt(PERI_WDT5_S, 0);
    }
    dsb();
    isb();

    if (nmi_transfer_add) {
        if (handle == NULL) {
            ERROR("[%s]:[%dL] handle NULL!!! \n\r", __func__, __LINE__);
            return TF_ERROR;
        }

        SaveContext(handle, linear_id, (uint64_t)id);
        write_ctx_reg(get_el3state_ctx(handle), CTX_ELR_EL3, nmi_transfer_add);

        /* 如果进入EL3前为EL0，需要返回至EL1去执行处理函数，此处是判断前状态是否为EL0,
        * 若是0则切换为EL1。同时中断BIT位需修改为MASK以防止中断产生。
        */
        spsr_el3 = read_ctx_reg(get_el3state_ctx(handle), CTX_SPSR_EL3);
        if ((0x0f & spsr_el3) == 0) {
            spsr_el3 |= 0x5;
            write_ctx_reg(get_el3state_ctx(handle), CTX_SPSR_EL3, spsr_el3);
        }

        spsr_el3 |= 0x02c0; /* not set A bit */
        write_ctx_reg(get_el3state_ctx(handle), CTX_SPSR_EL3, spsr_el3);
    }

    if (id == PERI_WDT5_S) {
        nmi_trigger_sgi_to_cores(FIQ_SGI_S);
    }
    dsb();
    isb();

    return TF_OK;
}

static uintptr_t NMI_private_handler(uint32_t fid, u_register_t x1, u_register_t x2, u_register_t x3, u_register_t x4,
                                     void *cookie, void *handle, u_register_t flags)
{
    switch (fid) {
        case PRIVATE_NMI_SMC_FIQ_GLUE_ID:
            INFO("Private SMC Call: 0x%x \n", fid);
            if ((void *)x1 != NULL) {
                nmi_transfer_add = x1;
                INFO("nmi_transfer_add= 0x%lx\n", nmi_transfer_add);
            } else {
                WARN("nmi_transfer_add is NULL\n");
            }

            if ((void *)x2 != NULL) {
                nmi_RTOS_context_addr = x2;
                INFO("nmi_RTOS_context_addr= 0x%lx\n", nmi_RTOS_context_addr);
            } else {
                WARN("nmi_RTOS_context_addr is NULL\n");
            }

            SMC_RET0(x1)
        /* fall-through */
        default:
            WARN("Unimplemented Private SMC Call: 0x%x \n", fid);
            SMC_RET1(handle, SMC_UNK)
    }
}

DECLARE_RT_SVC(Private, OEN_OEM_START, OEN_OEM_END, SMC_TYPE_FAST, NULL, NMI_private_handler);

