/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: usb driver.
 * Create: 2019-11-03
 */

#include <linux/version.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/spinlock.h>
#include <linux/io.h>
#include <linux/slab.h>
#include <linux/platform_device.h>
#include <linux/pm_runtime.h>
#include <linux/interrupt.h>
#include <linux/dma-mapping.h>
#include <linux/ioport.h>
#include <linux/list.h>
#include <linux/delay.h>
#include <linux/of.h>
#include <linux/pinctrl/consumer.h>
#include <linux/acpi.h>
#include <linux/proc_fs.h>
#include <linux/usb/of.h>
#include <linux/usb/ch9.h>
#include <linux/usb/gadget.h>
#include <linux/usb/otg.h>
#include <linux/uaccess.h>

#include "core.h"
#include "gadget.h"
#include "io.h"
#include "debug.h"
#include "comm_knl_api.h"
#include "usb_drv.h"
#include "drd_cdev.h"

#define DWC3_DRD_BUFSIZE    32

struct dwc3_drd {
    unsigned int usb_id;
    struct dwc3 *dwc;
    struct list_head entry;
    cdev_st dev_st;
    char dev_name[DWC3_DRD_BUFSIZE];
};

struct semaphore g_dwc3_drd_sem = __SEMAPHORE_INITIALIZER(g_dwc3_drd_sem, 1);
static struct list_head g_dwc3_drd_head = LIST_HEAD_INIT(g_dwc3_drd_head);

static int dwc3_drd_role_set(struct dwc3_drd *drd, void *arg)
{
    unsigned int mode = *((unsigned int *)arg);
    struct dwc3 *dwc = drd->dwc;

    if (!dwc || (dwc->dr_mode != USB_DR_MODE_OTG)) {
        return -EINVAL;
    }

    switch (mode) {
        case USB_DRD_HOST:
            dwc3_set_mode(dwc, DWC3_GCTL_PRTCAP_HOST);
            break;
        case USB_DRD_DEVICE:
            dwc3_set_mode(dwc, DWC3_GCTL_PRTCAP_DEVICE);
            break;
        case USB_DRD_OTG:
            dwc3_set_mode(dwc, DWC3_GCTL_PRTCAP_OTG);
            break;
        default:
            return -EINVAL;
    }

    return 0;
}

static int dwc3_drd_role_get(struct dwc3_drd *drd, void *arg)
{
    unsigned int *mode = (unsigned int *)arg;

    switch (drd->dwc->current_dr_role) {
        case DWC3_GCTL_PRTCAP_HOST:
            *mode = USB_DRD_HOST;
            return 0;
        case DWC3_GCTL_PRTCAP_DEVICE:
            *mode = USB_DRD_DEVICE;
            return 0;
        case DWC3_GCTL_PRTCAP_OTG:
            *mode = USB_DRD_OTG;
            return 0;
        default:
            return -EFAULT;
    }
}

static int dwc3_drd_open(struct inode *inodes, struct file *files)
{
    struct cdev *cdev = inodes->i_cdev;
    cdev_st *dev = container_of(cdev, cdev_st, cdev);
    struct dwc3_drd *drd = container_of(dev, struct dwc3_drd, dev_st);

    files->private_data = drd;
    return 0;
}

static int dwc3_drd_release(struct inode *inodes, struct file *files)
{
    files->private_data = NULL;
    return 0;
}

static long dwc3_drd_ioctl_process(struct dwc3_drd *drd, unsigned int cmd, void *arg)
{
    switch (cmd) {
        case USB_CMD_DRD_SET:
            return dwc3_drd_role_set(drd, arg);
        case USB_CMD_DRD_GET:
            return dwc3_drd_role_get(drd, arg);
        default:
            return -ENOSYS;
    }
}

static long dwc3_drd_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
    struct dwc3_drd *drd = filp->private_data;
    char cmd_buffer[DWC3_DRD_BUFSIZE] = {0};
    int ret;

    if (!drd) {
        return -EFAULT;
    }

    if ((_IOC_SIZE(cmd) > sizeof(cmd_buffer)) || !arg ||
        copy_from_user(cmd_buffer, (void *)arg, _IOC_SIZE(cmd))) {
        return -EFAULT;
    }

    down(&g_dwc3_drd_sem);
    ret = dwc3_drd_ioctl_process(drd, cmd, (void *)cmd_buffer);
    up(&g_dwc3_drd_sem);

    if (!ret && (_IOC_DIR(cmd) & _IOC_READ)) {
        if (copy_to_user((void *)arg, cmd_buffer, _IOC_SIZE(cmd))) {
            return -EFAULT;
        }
    }

    return ret;
}

static const struct file_operations g_dwc3_drd_ops = {
    .open = dwc3_drd_open,
    .release = dwc3_drd_release,
    .unlocked_ioctl = dwc3_drd_ioctl,
};

int dwc3_drd_init_user(struct dwc3 *dwc)
{
    struct dwc3_drd *drd = NULL;
    int ret;

    if (dwc->dr_mode != USB_DR_MODE_OTG) {
        return -EINVAL;
    }

    list_for_each_entry(drd, &g_dwc3_drd_head, entry) {
        if (drd->dwc == dwc) {
            return -EINVAL;
        }
    }

    drd = kzalloc(sizeof(*drd), GFP_KERNEL);
    if (!drd) {
        return -ENOMEM;
    }

    ret = usb_name2id(kobject_name(&dwc->dev->kobj), &drd->usb_id);
    if (ret) {
        kfree(drd);
        return ret;
    }

    (void)sprintf_s(drd->dev_name, sizeof(drd->dev_name) - 1,
                    "usbdrd%u", drd->usb_id);

    ret = comm_init_dev(&drd->dev_st, &g_dwc3_drd_ops, drd->dev_name);
    if (ret) {
        kfree(drd);
        return ret;
    }

    drd->dwc = dwc;
    list_add_tail(&drd->entry, &g_dwc3_drd_head);

    return ret;
}

void dwc3_drd_exit_user(struct dwc3 *dwc)
{
    struct dwc3_drd *drd = NULL;
    struct dwc3_drd *tmp_drd = NULL;

    list_for_each_entry_safe(drd, tmp_drd, &g_dwc3_drd_head, entry) {
        if (drd->dwc == dwc) {
            comm_cleanup_dev(&drd->dev_st);
            list_del(&drd->entry);
            kfree(drd);
        }
    }

    return;
}
