/**
 * @file
 *
 * @date Sep 25, 2019
 * @author Anton Bondarev
 */

#include <errno.h>
#include <stddef.h>
#include <stdint.h>

#include <drivers/common/memory.h>
#include <drivers/i2c/i2c.h>
#include <util/log.h>

#include "i2c_designware.h"

#define I2C_DW_BASE    OPTION_GET(NUMBER, base_addr)
#define I2C_DW_IRQ_NUM OPTION_GET(NUMBER, irq_num)
#define I2C_DW_ID      1

static struct i2c_dw_dev i2c_dw1_priv = {
    .irq_num = I2C_DW_IRQ_NUM,
    .base_addr = I2C_DW_BASE,
};

static const struct i2c_bus i2c_dw1_bus = {
    .i2c_priv = &i2c_dw1_priv,
    .i2c_ops = &i2c_dw_ops,
    .i2c_id = I2C_DW_ID,
};

I2C_BUS_REGISTER(&i2c_dw1_bus);

PERIPH_MEMORY_DEFINE(i2c_dw1, I2C_DW_BASE, 0x100);
