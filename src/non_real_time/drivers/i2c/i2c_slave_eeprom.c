// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * I2C slave mode EEPROM simulator.
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#include "i2c_dw.h"
#include "i2c_api.h"

enum {
    I2C_SLAVE_EEPROM_BANKWIDTH8 = 1,
    I2C_SLAVE_EEPROM_BANKWIDTH16,
};

struct i2c_eeprom_control {
    unsigned int first_write;
    unsigned int offset;
    unsigned int buffer_len;
    unsigned char *buffer;
} __attribute__((aligned(sizeof(long))));

static inline void i2c_slave_eeprom_offset_inc(struct i2c_eeprom_control *eeprom)
{
    eeprom->offset++;
    eeprom->offset %= eeprom->buffer_len;
}

static inline int i2c_slave_eeprom_offset(struct i2c_eeprom_control *eeprom)
{
    return eeprom->offset % eeprom->buffer_len;
}

static unsigned int i2c_slave_eeprom_bankwidth(unsigned int value)
{
    /*
     * If the buffer size is less than 256 bytes, the bit width of the device is set to 1 byte.
     * Otherwise, the bit width of the device is set to 2 bytes.
     */
    return (value <= 256) ? I2C_SLAVE_EEPROM_BANKWIDTH8 : I2C_SLAVE_EEPROM_BANKWIDTH16;
}

static inline void i2c_slave_eeprom_init(struct i2c_eeprom_control *eeprom)
{
    eeprom->offset = 0;
    eeprom->first_write = i2c_slave_eeprom_bankwidth(eeprom->buffer_len);
}

static void i2c_slave_eeprom_write(struct i2c_adapter *adap, struct i2c_eeprom_control *eeprom)
{
    unsigned int max_bankwidth;
    unsigned int shift;
    unsigned int rxflr, index;
    unsigned char val;

    rxflr = i2c_readl(adap, ICR_RXFLR);
    for (index = 0; index < rxflr; index++) {
        val = (unsigned char)i2c_readl(adap, ICR_DATA_CMD);
        if (eeprom->first_write) {
            max_bankwidth = i2c_slave_eeprom_bankwidth(eeprom->buffer_len);
            eeprom->first_write = min_t(unsigned int, max_bankwidth, eeprom->first_write);
            shift = ((eeprom->first_write - 1) % sizeof(unsigned int)) * 8; // each byte has 8 bit
            eeprom->offset &= ~(0xFFU << shift);
            eeprom->offset |= (unsigned int)((unsigned int)(val) << shift);
            eeprom->first_write--;
        } else {
            eeprom->buffer[i2c_slave_eeprom_offset(eeprom)] = val;
            i2c_slave_eeprom_offset_inc(eeprom);
        }
    }
}

static int i2c_slave_eeprom_handle(struct i2c_adapter *adap, enum i2c_slave_event event)
{
    struct i2c_eeprom_control *eeprom = adap->slave_priv;
    unsigned char val = 0;

    switch (event) {
        case I2C_SLAVE_WRITE_RECEIVED:
            i2c_slave_eeprom_write(adap, eeprom);
            break;
        case I2C_SLAVE_READ_PROCESSED:
            i2c_slave_eeprom_offset_inc(eeprom);
            val = eeprom->buffer[i2c_slave_eeprom_offset(eeprom)];
            i2c_writel(adap, val, ICR_DATA_CMD);
            break;
        case I2C_SLAVE_READ_REQUESTED:
            val = eeprom->buffer[i2c_slave_eeprom_offset(eeprom)];
            i2c_writel(adap, val, ICR_DATA_CMD);
            break;
        case I2C_SLAVE_STOP:
            if (i2c_readl(adap, ICR_RXFLR)) {
                i2c_slave_eeprom_write(adap, eeprom);
            }
            i2c_slave_eeprom_init(eeprom);
            break;
        case I2C_SLAVE_ABORT:
        case I2C_SLAVE_WRITE_REQUESTED:
            i2c_slave_eeprom_init(eeprom);
            break;
        default:
            return -I2C_EDEFAULT;
    }

    return 0;
}

static void i2c_slave_eeprom_control_init(struct i2c_adapter *adap)
{
    struct i2c_eeprom_control *eeprom = adap->slave_priv;

    i2c_writel(adap, 0, ICR_TX_TL);
    i2c_writel(adap, 0, ICR_RX_TL);

    eeprom = adap->slave_priv;
    eeprom->buffer = adap->slave_data;
    eeprom->buffer_len = adap->slave_dlen;
    i2c_slave_eeprom_init(eeprom);
}

static int i2c_slave_eeprom_slave_cb(struct i2c_adapter *adap, enum i2c_slave_event event)
{
    unsigned long flags = 0;
    int ret;

    if (!adap->slave_priv || !adap->slave_data || !adap->slave_dlen) {
        return -I2C_EDEFAULT;
    }

    spin_lock_irqsave(&adap->lock, flags);
    ret = i2c_slave_eeprom_handle(adap, event);
    spin_unlock_irqrestore(&adap->lock, flags);

    return ret;
}

struct i2c_slave g_slave_eeprom = {
    .name = "eeprom",
    .control_memlen = sizeof(struct i2c_eeprom_control),
    .slave_cb = i2c_slave_eeprom_slave_cb,
    .slave_init = i2c_slave_eeprom_control_init,
};
