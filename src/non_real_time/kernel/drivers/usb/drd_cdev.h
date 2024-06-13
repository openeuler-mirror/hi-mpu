/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: usb driver.
 * Create: 2019-11-03
 */

#ifndef   __DWC3_DRD_H__
#define   __DWC3_DRD_H__

/* Use 'u' as magic number */
#define USB_DRD_IOC_MAGIC  'u'
#define USB_CMD_DRD_SET _IOW(USB_DRD_IOC_MAGIC, 0x1, int)
#define USB_CMD_DRD_GET _IOR(USB_DRD_IOC_MAGIC, 0x2, int)

#define USB_DRD_DEVICE  0
#define USB_DRD_HOST    1
#define USB_DRD_OTG     2

extern int  dwc3_drd_init_user(struct dwc3 *dwc);
extern void dwc3_drd_exit_user(struct dwc3 *dwc);

#endif /* __DWC3_DRD_H__ */
