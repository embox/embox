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

EMBOX_UNIT_INIT(stm32_i2c1_init);

#define USE_I2C_IRQ \
	OPTION_MODULE_GET(embox__driver__i2c__stm32_i2c_f4, BOOLEAN, use_i2c_irq)

static I2C_HandleTypeDef i2c1_handle;

static struct stm32_i2c stm32_i2c1_priv = {
	.i2c_handle = &i2c1_handle,
	.i2c = I2C1,
	.event_irq = I2C1_EVENT_IRQ,
	.error_irq = I2C1_ERROR_IRQ,
};

static struct i2c_adapter stm32_i2c1_adap = {
	.i2c_algo_data = &stm32_i2c1_priv,
	.i2c_algo = &stm32_i2c_algo,
};

static void stm32_i2c_gpio_init(I2C_HandleTypeDef *hi2c) {
	gpio_setup_mode(I2C1_SCL_PORT, I2C1_SCL_PIN,
		GPIO_MODE_OUT_ALTERNATE | GPIO_ALTERNATE(I2C1_SCL_AF) |
		GPIO_MODE_OUT_OPEN_DRAIN | GPIO_MODE_IN_PULL_UP);

	gpio_setup_mode(I2C1_SDA_PORT, I2C1_SDA_PIN,
		GPIO_MODE_OUT_ALTERNATE | GPIO_ALTERNATE(I2C1_SDA_AF) |
		GPIO_MODE_OUT_OPEN_DRAIN | GPIO_MODE_IN_PULL_UP);
}

static int stm32_i2c1_init(void) {
	if (0 > stm32_i2c_common_init(&stm32_i2c1_priv)) {
		return -1;
	}

	i2c1_enable_gpio_clocks();
	stm32_i2c_gpio_init(&i2c1_handle);
	i2c1_enable_i2c_clocks();

	return i2c_bus_register(&stm32_i2c1_adap, 1, "i2c1");
}

#if USE_I2C_IRQ
extern irq_return_t i2c_ev_irq_handler(unsigned int irq_nr, void *data);
extern irq_return_t i2c_er_irq_handler(unsigned int irq_nr, void *data);

STATIC_IRQ_ATTACH(I2C1_EVENT_IRQ, i2c_ev_irq_handler, &i2c1_handle);
STATIC_IRQ_ATTACH(I2C1_ERROR_IRQ, i2c_er_irq_handler, &i2c1_handle);
#endif
