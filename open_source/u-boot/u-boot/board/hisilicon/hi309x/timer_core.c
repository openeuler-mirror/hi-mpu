/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2025-2026. All rights reserved.
 * Description: TIMER hw adapter APIs
 * Create: 2025-01-09
 */
#include <errno.h>
#include <asm/io.h>
#include "timer_core.h"

static const unsigned long g_reg_bases[] = {
    TIMER0_REG_BASE,
    TIMER1_REG_BASE,
    TIMER2_REG_BASE,
    TIMER3_REG_BASE,
    TIMER4_REG_BASE,
    TIMER5_REG_BASE,
};

static void timer_config(const timer_cfg *cfg)
{
    timer_reg_struct *regs = (timer_reg_struct *)g_reg_bases[cfg->id];

    regs->timer_intclr = 0x01;
    regs->timer_load_l = cfg->load_l;
    regs->timer_load_h = cfg->load_h;
    regs->timer_bgload_l = cfg->bgload_l;
    regs->timer_bgload_h = cfg->bgload_h;

    regs->timer_control.bit.intenable = cfg->interrupten;
    regs->timer_control.bit.timerpre = cfg->prescaler;
	regs->timer_control.bit.timersize = cfg->size;
    if (cfg->mode == 0x02) {
        regs->timer_control.bit.oneshot = 0x01;
    } else {
        regs->timer_control.bit.oneshot = 0x00;
        regs->timer_control.bit.timermode = (cfg->mode == 0x00) ? 0x00 : 0x01;
    }
}

int timer_core_init(const timer_cfg *cfg)
{
    if (cfg->id >= 6 || cfg->id < 0 || cfg == NULL) {
        return -EINVAL;
    }

    timer_config(cfg);

    return 0;
}

void timer_cnt(unsigned int id, unsigned int load_l)
{
    timer_reg_struct *regs = (timer_reg_struct *)g_reg_bases[id];
    regs->timer_load_l = load_l;
    regs->timer_control.bit.timeren = 1;

    //Wait for the delay time to end
    while (regs->timer_ris.timerris == 0) {
    }

    regs->timer_control.bit.timeren = 0;
    regs->timer_intclr = 1;

    return;
}
