/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 29.05.25
 */

#include <drivers/common/memory.h>
#include <drivers/i2c/i2c.h>

#include "sc64_i2c.h"

#define BASE_ADDR  OPTION_GET(NUMBER, base_addr)
#define I2C_BUS_ID 1

static const struct i2c_bus sc64_i2c1_bus = {
    .i2c_priv = (void *)BASE_ADDR,
    .i2c_ops = &sc64_i2c_ops,
    .i2c_id = I2C_BUS_ID,
};

I2C_BUS_REGISTER(&sc64_i2c1_bus);
