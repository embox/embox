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

#include <framework/mod/options.h>

#define I2C_BUS_ID  0

extern const struct i2c_ops vg015_i2c_ops;

#if 0
#include <drivers/pin_description.h>
static const struct pin_description vg015_i2c_pins[] = {
	{SCLK_PORT, SCLK_PIN, SCLK_AF},
	{RX_PORT, RX_PIN, RX_AF},
};

#endif

static const struct i2c_bus i2c_bus0 = {
    .i2c_priv = NULL,
    .i2c_ops = &vg015_i2c_ops,
    .i2c_id = I2C_BUS_ID,
};

I2C_BUS_REGISTER(&i2c_bus0);

