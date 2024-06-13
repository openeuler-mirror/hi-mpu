/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2021-2024
 */

#ifndef __KBOX_H__
#define __KBOX_H__

#ifdef __KERNEL__
#include <asm/types.h>
#include <linux/errno.h> /* error codes */
#include <linux/types.h> /* size_t */

/* here is unused .h for kbox, but we can't del
 * all outside interface can't change ... */
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/rtc.h>

#endif

#define KBOX_REGION_NAME_LEN	(32)
#define KBOX_DEFAULT_REG_ID (0)
#define KBOX_NULL_REGION_ID 0xffff

struct kbox_region {
    char name[KBOX_REGION_NAME_LEN];
    size_t size;
    struct module *mod;
    int rfd;
};

/* Ioctl definitions */
/* Use 'K' as magic number */
#define KBOX_IOC_MAGIC  'k'
/* create region */
#define KBOX_IOC_CREATE_REGION			_IOWR(KBOX_IOC_MAGIC, 1,  char)

#ifdef __KERNEL__
/*
kbox region register/unregister
*******************************************************************************
*/
#ifdef CONFIG_KBOX
extern asmlinkage int kbox_write(int fd, const char *buff, unsigned len);
extern int kbox_create_region(struct kbox_region *reg_interface);
extern unsigned int kbox_get_region_size(const char *region_name);
extern int kbox_region_forbidden_record_time(int rfd);

#define kbox_register_region(_reg) \
    ({ \
        _reg.mod = THIS_MODULE; \
        kbox_create_region(&_reg); \
    })

#define kbox_unregister_region(_reg) \
    ({ \
    })
#else
static inline int kbox_write(int fd, const char *buff, unsigned len)
{
    return len;
}

static inline time64_t kbox_ktime_get_real_seconds(void)
{
    return 0;
}

static inline int kbox_register_region(struct kbox_region reg_interface)
{
    return -1;
}

static inline unsigned int kbox_get_region_size(const char *region_name)
{
    return 0;
}
#endif

#endif

#endif
