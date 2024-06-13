// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Definitions for i2c-core interface
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#ifndef __I2C_CORE_H__
#define __I2C_CORE_H__

#include "i2c.h"
#include "i2c_api.h"

extern int i2c_xfer(struct i2c_adapter *adap, struct i2c_kmsg *msg);
extern int i2c_smbus_xfer(struct i2c_adapter *adap, struct i2c_kmsg *msg);

#endif /* __I2C_CORE_H__ */
