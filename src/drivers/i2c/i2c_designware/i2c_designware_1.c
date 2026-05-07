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
#include <util/log.h>

#include "../i2c_priv.h"
#include "i2c_designware.h"

#define I2C_DW_BASE    OPTION_GET(NUMBER, base_addr)
#define I2C_DW_IRQ_NUM OPTION_GET(NUMBER, irq_num)
#define I2C_DW_ID      1
#define USE_BOARD_CONF OPTION_GET(BOOLEAN, use_board_conf)

#if USE_BOARD_CONF
#include <drivers/gpio.h>
#include <drivers/pin_description.h>

#include <config/board_config.h>

static const struct pin_description dw_i2c_pins[] = {
	{CONF_I2C1_PIN_SCL_PORT, CONF_I2C1_PIN_SCL_NR, CONF_I2C1_PIN_SCL_AF},
	{CONF_I2C1_PIN_SDA_PORT, CONF_I2C1_PIN_SDA_NR, CONF_I2C1_PIN_SDA_AF},
};
#endif

static struct i2c_dw_dev i2c_dw1_priv = {
    .irq_num = I2C_DW_IRQ_NUM,
    .base_addr = I2C_DW_BASE,
};

static const struct i2c_bus i2c_dw1_bus = {
    .i2cb_priv = &i2c_dw1_priv,
    .i2cb_ops = &i2c_dw_ops,
    .i2cb_id = I2C_DW_ID,
#if USE_BOARD_CONF
    .i2cb_pins = dw_i2c_pins,
#endif
};

I2C_BUS_REGISTER(&i2c_dw1_bus);

PERIPH_MEMORY_DEFINE(i2c_dw1, I2C_DW_BASE, 0x100);
