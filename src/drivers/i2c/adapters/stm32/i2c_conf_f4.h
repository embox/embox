/**
 * @file
 *
 * @date    10.12.2018
 * @author  Alex Kalmuk
 */

#ifndef SRC_DRIVERS_ADAPTERS_STM32_I2C_CONF_F4_H_
#define SRC_DRIVERS_ADAPTERS_STM32_I2C_CONF_F4_H_

#include "stm32f4_discovery.h"
#include <drivers/gpio/gpio.h>

/* I2C1 */
#define I2C1_SCL_PIN  GPIO_PIN_6
#define I2C1_SDA_PIN  GPIO_PIN_9
#define I2C1_SCL_AF   GPIO_AF4_I2C1
#define I2C1_SDA_AF   GPIO_AF4_I2C1
#define I2C1_SCL_PORT GPIO_PORT_B
#define I2C1_SDA_PORT GPIO_PORT_B

#define I2C1_EVENT_IRQ  47
#define I2C1_ERROR_IRQ  48

static inline void i2c1_enable_gpio_clocks(void) {
	__HAL_RCC_GPIOB_CLK_ENABLE();
}
static inline void i2c1_enable_i2c_clocks(void) {
	__HAL_RCC_I2C1_CLK_ENABLE();
}

/* I2C2 */
#define I2C2_SCL_PIN  GPIO_PIN_10
#define I2C2_SDA_PIN  GPIO_PIN_11
#define I2C2_SCL_AF   GPIO_AF4_I2C2
#define I2C2_SDA_AF   GPIO_AF4_I2C2
#define I2C2_SCL_PORT GPIO_PORT_B
#define I2C2_SDA_PORT GPIO_PORT_B

#define I2C2_EVENT_IRQ  49
#define I2C2_ERROR_IRQ  50

static inline void i2c2_enable_gpio_clocks(void) {
	__HAL_RCC_GPIOB_CLK_ENABLE();
}
static inline void i2c2_enable_i2c_clocks(void) {
	__HAL_RCC_I2C2_CLK_ENABLE();
}

#endif /* SRC_DRIVERS_ADAPTERS_STM32_I2C_CONF_F4_H_ */
