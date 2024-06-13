/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2020. All rights reserved.
 * Description: Hi309x uboot header file for platform definations
 * Create: 2018-05-10
 */

#ifndef __HI309x_TYPES_H
#define __HI309x_TYPES_H

#ifndef ERROR
#define ERROR    (-1)
#endif

#ifndef OK
#define OK      0
#endif

#ifndef NULL
#define NULL ((void *)0)
#endif

#define BYTE_SWAP32(x)      (x)
#define BYTE_SWAP16(x)      (x)

/* 寄存器操作的宏定义 */
#ifndef REG8
#define REG8(Addr) (*(volatile unsigned char *const)(Addr))
#endif

#ifndef REG16
#define REG16(Addr) (*(volatile unsigned short *const)(Addr))
#endif

#ifndef REG32
#define REG32(Addr) (*(volatile unsigned int *const)(Addr))
#endif

#endif
