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
#include <drivers/gpio.h>

#include <framework/mod/options.h>
#include <config/board_config.h>

#define I2C_BUS_ID  0

#define BASE_ADDR   OPTION_GET(NUMBER, base_addr)

extern const struct i2c_ops vg015_i2c_ops;

#include <drivers/pin_description.h>

extern int clk_enable(char *clk_name);

static const struct pin_description vg015_i2c_pins[] = {
	{CONF_I2C0_PIN_SCL_PORT, CONF_I2C0_PIN_SCL_NR, CONF_I2C0_PIN_SCL_NR},
	{CONF_I2C0_PIN_SCL_PORT, CONF_I2C0_PIN_SCL_NR, CONF_I2C0_PIN_SDA_AF},
};

int vg015_i2c_hw_init0(const struct i2c_bus *bus) {
    clk_enable(CONF_I2C0_CLK_DEF);
	
    return 0;
}

static const struct i2c_bus i2c_bus0 = {
    .i2c_priv = NULL,
    .i2c_ops = &vg015_i2c_ops,
    .i2c_id = I2C_BUS_ID,
    .i2cb_pins = vg015_i2c_pins,
    .i2cb_label = BASE_ADDR,
};

I2C_BUS_REGISTER(&i2c_bus0);

