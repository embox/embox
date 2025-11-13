/**
 * @file
 *
 * @date Oct 23, 2025
 * @author Anton Bondarev
 */

 #include <util/log.h>

#include <errno.h>
#include <stddef.h>
#include <stdint.h>

#include <drivers/i2c/i2c.h>

#define I2C_BUS_ID  0

extern const struct i2c_ops skeleton_i2c_ops;

static const struct i2c_bus i2c_bus0 = {
    .i2c_priv = NULL,
    .i2c_ops = &skeleton_i2c_ops,
    .i2c_id = I2C_BUS_ID,
};

I2C_BUS_REGISTER(&i2c_bus0);

