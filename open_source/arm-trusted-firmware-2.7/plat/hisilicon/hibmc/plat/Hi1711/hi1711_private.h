/* Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: hi1711_private.h
 * Author: xxx
 * Create: 2022-11-16
 */

#ifndef __HI1711_PRIVATE_H__
#define __HI1711_PRIVATE_H__

#include <common/bl_common.h>
#include <drivers/io/io_memmap.h>
#include <bl31/interrupt_mgmt.h>

typedef struct hi1711_bl31_params {
    param_header_t h;
    image_info_t *bl31_image_info;
    entry_point_info_t *bl32_ep_info;
    image_info_t *bl32_image_info;
    entry_point_info_t *bl33_ep_info;
    image_info_t *bl33_image_info;
} hi1711_bl31_params_t;

unsigned int plat_arm_calc_core_pos(u_register_t mpidr);

void plat_arm_gic_redistif_on(void);
void plat_arm_gic_redistif_off(void);
void plat_arm_gic_cpuif_enable(void);
void plat_arm_gic_cpuif_disable(void);
void plat_arm_gic_save(void);
void plat_arm_gic_resume(void);
void plat_arm_gic_driver_init(void);
void plat_arm_gic_init(void);
void plat_arm_gic_pcpu_init(void);

void arm_io_setup(void);

void nop(void);

/* Pointer of fun, which init sth before register the gic */
typedef void (*INITFUN_BEFORE_REGISTER)(void);

typedef struct gic_register_para {
    uint32_t source;
    interrupt_type_handler_t handler;
    INITFUN_BEFORE_REGISTER init_fun;
} interrupt_handler_table_t;

uint32_t IsAsicBoot();
uint64_t el3_interrupt_handler(uint32_t id, uint32_t flags, void *handle, void *cookie);
uint64_t ext_int0_int_handler(uint32_t id, uint32_t flags, void *handle, void *cookie);
void gicd_set_icfgr(uintptr_t base, unsigned int id, unsigned int cfg);

#endif /* __HI1711_PRIVATE_H__ */
