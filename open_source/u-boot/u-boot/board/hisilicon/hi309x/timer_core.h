/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2025-2026. All rights reserved.
 * Description: Hi3093 TIMER core functions declaration header file
 * Create: 2025-01-09
 */
#ifndef __TIMER_CORE_H__
#define __TIMER_CORE_H__

#define TIMER0_REG_BASE 0x08700000
#define TIMER1_REG_BASE 0x08700040
#define TIMER2_REG_BASE 0x08702000
#define TIMER3_REG_BASE 0x08702040
#define TIMER4_REG_BASE 0x08704000
#define TIMER5_REG_BASE 0x08704040

#define TIMER_1US                            100
#define CONFIG_SYS_TIMER_NUM                 3

/**
 * @brief timer control register structure
 */
typedef union {
    unsigned int reg;
    struct {
        unsigned int oneshot : 1;
        unsigned int timersize : 1;
        unsigned int timerpre : 2;
        unsigned int reserve1 : 1;
        unsigned int intenable : 1;
        unsigned int timermode : 1;
        unsigned int timeren : 1;
        unsigned int reserve2 : 24;
    } bit;
} timer_control_reg;

/**
 * @brief timer original interrupt register
 */
typedef struct {
    unsigned int timerris : 1;
    unsigned int reserve : 31;
} timer_ris_reg;

/**
 * @brief timer interrupt register of shield
 */
typedef struct {
    unsigned int timermis : 1;
    unsigned int reserve : 31;
} timer_mis_reg;

/**
 * @brief timer register structure
 */
typedef struct {
    unsigned int timer_load_l;
    unsigned int timer_load_h;
    unsigned int timer_value_l;
    unsigned int timer_value_h;
    timer_control_reg timer_control;
    unsigned int timer_intclr;
    timer_ris_reg timer_ris;
    timer_mis_reg timer_mis;
    unsigned int timer_bgload_l;
    unsigned int timer_bgload_h;
} volatile timer_reg_struct;

typedef struct {
    unsigned int id;                  /* *<timer id */
    unsigned int load_l;        /* *< period, set the timer_load low 32bit */
    unsigned int load_h;        /* *< period, set the timer_load high 32bit */
    unsigned int mode;                  /* *< timer counting mode */
    unsigned int interrupten;         /* *< interrupt enable or disable */
    unsigned int prescaler; /* *< timer prescaler */
    unsigned int size;                  /* *< timer size 32 or 64 bits */
    unsigned int bgload_l;      /* *< Backgroud period, set the timer_bgload low 32bit */
    unsigned int bgload_h;      /* *< Backgroud period, set the timer_bgload high 32bit */
} timer_cfg;

extern int timer_core_init(const timer_cfg *cfg);
extern void timer_cnt(unsigned int id, unsigned int load_l);

#endif /* __TIMER_CORE_H__ */
