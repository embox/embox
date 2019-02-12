/**
 * @file
 * @brief I2C MSP layer derived from Cube
 *
 * @date    29.03.2017
 * @author  Alex Kalmuk
 */

#include <errno.h>
#include <string.h>
#include <assert.h>
#include <util/log.h>
#include <embox/unit.h>
#include <kernel/irq.h>

#include "stm32f4_discovery.h"

static int i2cx_scl_pin(void *i2c) {
	assert(i2c == I2C1);
	return GPIO_PIN_6;
}

static int i2cx_sda_pin(void *i2c) {
	assert(i2c == I2C1);
	return GPIO_PIN_9;
}

static int i2cx_scl_af(void *i2c) {
	assert(i2c == I2C1);
	return GPIO_AF4_I2C1;
}

static int i2cx_sda_af(void *i2c) {
	assert(i2c == I2C1);
	return GPIO_AF4_I2C1;
}

static GPIO_TypeDef *i2cx_scl_port(void *i2c) {
	assert(i2c == I2C1);
	return GPIOB;
}

static GPIO_TypeDef *i2cx_sda_port(void *i2c) {
	assert(i2c == I2C1);
	return GPIOB;
}

static void i2cx_enable_gpio_clocks(void *i2c) {
	assert(i2c == I2C1);
	__HAL_RCC_GPIOB_CLK_ENABLE();
}

static void i2cx_enable_periph_clocks(void *i2c) {
	assert(i2c == I2C1);
	__HAL_RCC_I2C1_CLK_ENABLE();
}

void HAL_I2C_MspInit(I2C_HandleTypeDef *hi2c) {
	GPIO_InitTypeDef GPIO_InitStruct;
	I2C_TypeDef *i2c;

	memset(&GPIO_InitStruct, 0, sizeof(GPIO_InitStruct));

	log_debug("");

	i2c = hi2c->Instance;

	/*##-1- Enable GPIO Clocks #################################################*/
	/* Enable GPIO TX/RX clock */
	i2cx_enable_gpio_clocks(i2c);

	/*##-2- Configure peripheral GPIO ##########################################*/
	/* I2C TX GPIO pin configuration  */
	GPIO_InitStruct.Pin       = i2cx_scl_pin(i2c);
	GPIO_InitStruct.Mode      = GPIO_MODE_AF_OD;
	GPIO_InitStruct.Pull      = GPIO_PULLUP;
	GPIO_InitStruct.Speed     = GPIO_SPEED_FAST;
	GPIO_InitStruct.Alternate = i2cx_scl_af(i2c);
	HAL_GPIO_Init(i2cx_scl_port(i2c), &GPIO_InitStruct);

	/* I2C RX GPIO pin configuration  */
	GPIO_InitStruct.Pin = i2cx_sda_pin(i2c);
	GPIO_InitStruct.Alternate = i2cx_sda_af(i2c);
	HAL_GPIO_Init(i2cx_sda_port(i2c), &GPIO_InitStruct);

	/*##-3- Enable I2C peripheral Clock ########################################*/
	/* Enable I2C1 clock */
	i2cx_enable_periph_clocks(i2c);
}
