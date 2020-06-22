/**
 * @file
 *
 * @date    10.12.2018
 * @author  Alexander Kalmuk
 */
#include <string.h>
#include <drivers/i2c/i2c.h>
#include <kernel/irq.h>

#include <embox/unit.h>
#include <framework/mod/options.h>
#include <module/embox/driver/i2c/stm32_i2c_f4.h>

#include "stm32_i2c.h"

EMBOX_UNIT_INIT(stm32_i2c2_init);

#define USE_I2C_IRQ \
	OPTION_MODULE_GET(embox__driver__i2c__stm32_i2c_f4, BOOLEAN, use_i2c_irq)

static I2C_HandleTypeDef i2c2_handle;

static struct stm32_i2c stm32_i2c2_priv = {
	.i2c_handle = &i2c2_handle,
	.i2c = I2C2,
	.event_irq = I2C2_EVENT_IRQ,
	.error_irq = I2C2_ERROR_IRQ,
};

static struct i2c_adapter stm32_i2c2_adap = {
	.i2c_algo_data = &stm32_i2c2_priv,
	.i2c_algo = &stm32_i2c_algo,
};

static void stm32_i2c_gpio_init(I2C_HandleTypeDef *hi2c) {
	gpio_setup_mode(I2C2_SCL_PORT, I2C2_SCL_PIN,
		GPIO_MODE_OUT_ALTERNATE | GPIO_ALTERNATE(I2C2_SCL_AF) |
		GPIO_MODE_OUT_OPEN_DRAIN | GPIO_MODE_IN_PULL_UP);

	gpio_setup_mode(I2C2_SDA_PORT, I2C2_SDA_PIN,
		GPIO_MODE_OUT_ALTERNATE | GPIO_ALTERNATE(I2C2_SDA_AF) |
		GPIO_MODE_OUT_OPEN_DRAIN | GPIO_MODE_IN_PULL_UP);
}

static int stm32_i2c2_init(void) {
	if (0 > stm32_i2c_common_init(&stm32_i2c2_priv)) {
		return -1;
	}

	i2c2_enable_gpio_clocks();
	stm32_i2c_gpio_init(&i2c2_handle);
	i2c2_enable_i2c_clocks();

	return i2c_bus_register(&stm32_i2c2_adap, 2, "i2c2");
}

#if USE_I2C_IRQ
extern irq_return_t i2c_ev_irq_handler(unsigned int irq_nr, void *data);
extern irq_return_t i2c_er_irq_handler(unsigned int irq_nr, void *data);

STATIC_IRQ_ATTACH(I2C2_EVENT_IRQ, i2c_ev_irq_handler, &i2c2_handle);
STATIC_IRQ_ATTACH(I2C2_ERROR_IRQ, i2c_er_irq_handler, &i2c2_handle);
#endif
