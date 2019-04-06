/**
 * @file
 *
 * @date    10.12.2018
 * @author  Alexander Kalmuk
 */
#include <string.h>
#include <drivers/i2c/i2c.h>

#include <embox/unit.h>

#include "stm32_i2c.h"

EMBOX_UNIT_INIT(stm32_i2c1_init);

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
