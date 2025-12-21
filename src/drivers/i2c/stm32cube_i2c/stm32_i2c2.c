/**
 * @file
 *
 * @date    10.12.2018
 * @author  Alexander Kalmuk
 */

#include <assert.h>
#include <string.h>

#include <bsp/stm32cube_hal.h>
#include <drivers/gpio.h>
#include <drivers/i2c/i2c.h>
#include <framework/mod/options.h>
#include <kernel/irq.h>

#include <config/board_config.h>

#include "stm32_i2c.h"

#define IMX_I2C_ID 2

static void stm32_i2c2_init_pins(void) {
	gpio_setup_mode(CONF_I2C2_PIN_SCL_PORT, 1 << CONF_I2C2_PIN_SCL_NR,
	    GPIO_MODE_ALT_SET(CONF_I2C2_PIN_SCL_AF) | GPIO_MODE_OUT_OPEN_DRAIN
	        | GPIO_MODE_IN_PULL_UP);

	/* CONF_I2C1_CLK_ENABLE_SDA(); */
	gpio_setup_mode(CONF_I2C2_PIN_SDA_PORT, 1 << CONF_I2C2_PIN_SDA_NR,
	    GPIO_MODE_ALT_SET(CONF_I2C2_PIN_SDA_AF) | GPIO_MODE_OUT_OPEN_DRAIN
	        | GPIO_MODE_IN_PULL_UP);

	CONF_I2C2_CLK_ENABLE_I2C();
}

static I2C_HandleTypeDef i2c2_handle;

static struct stm32_i2c_priv stm32_i2c2_priv = {
    .init_pins = stm32_i2c2_init_pins,
    .i2c_handle = &i2c2_handle,
#if defined CONF_I2C1_REGION_BASE
    .i2c = (I2C_TypeDef *)CONF_I2C2_REGION_BASE,
#else
    .i2c = I2C2,
#endif /* CONF_I2C2_REGION_BASE */
    .event_irq = CONF_I2C2_IRQ_EVENT,
    .error_irq = CONF_I2C2_IRQ_ERROR,
};

static const struct i2c_bus stm32_i2c2_bus = {
    .i2c_priv = &stm32_i2c2_priv,
    .i2c_ops = &stm32_i2c_ops,
    .i2c_id = IMX_I2C_ID,
};

I2C_BUS_REGISTER(&stm32_i2c2_bus);

#if USE_I2C_IRQ
extern irq_return_t i2c_ev_irq_handler(unsigned int irq_nr, void *data);
extern irq_return_t i2c_er_irq_handler(unsigned int irq_nr, void *data);

static_assert(CONF_I2C2_IRQ_EVENT == I2C1_EV_IRQn, "");
static_assert(CONF_I2C2_IRQ_ERROR == I2C1_EV_IRQn, "");

STATIC_IRQ_ATTACH(CONF_I2C2_IRQ_EVENT, i2c_ev_irq_handler, &i2c2_handle);
STATIC_IRQ_ATTACH(CONF_I2C2_IRQ_ERROR, i2c_er_irq_handler, &i2c2_handle);
#endif
