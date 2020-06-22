/**
 * @file
 *
 * @date    10.12.2018
 * @author  Alex Kalmuk
 */

#ifndef SRC_DRIVERS_ADAPTERS_STM32_I2C_CONF_F4_H_
#define SRC_DRIVERS_ADAPTERS_STM32_I2C_CONF_F4_H_

#if defined(STM32F407xx)
#include "stm32f4_discovery.h"
#elif defined (STM32F429xx)
#include "stm32f4xx_nucleo_144.h"
#else
#error Unsupported platform
#endif
#include <drivers/gpio/gpio.h>
#include <assert.h>
#include <framework/mod/options.h>
#include <module/embox/driver/i2c/stm32_i2c_f4.h>

/* I2C1 */
#define I2C1_SCL_PIN  GPIO_PIN_6
#define I2C1_SDA_PIN  GPIO_PIN_9
#define I2C1_SCL_AF   GPIO_AF4_I2C1
#define I2C1_SDA_AF   GPIO_AF4_I2C1
#define I2C1_SCL_PORT GPIO_PORT_B
#define I2C1_SDA_PORT GPIO_PORT_B

#define I2C1_EVENT_IRQ  \
	OPTION_MODULE_GET(embox__driver__i2c__stm32_i2c_f4, NUMBER, i2c1_ev_irq)
static_assert(I2C1_EVENT_IRQ == I2C1_EV_IRQn);

#define I2C1_ERROR_IRQ \
	OPTION_MODULE_GET(embox__driver__i2c__stm32_i2c_f4, NUMBER, i2c1_er_irq)
static_assert(I2C1_ERROR_IRQ == I2C1_ER_IRQn);

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

#define I2C2_EVENT_IRQ  \
	OPTION_MODULE_GET(embox__driver__i2c__stm32_i2c_f4, NUMBER, i2c2_ev_irq)
static_assert(I2C2_EVENT_IRQ == I2C2_EV_IRQn);

#define I2C2_ERROR_IRQ  \
	OPTION_MODULE_GET(embox__driver__i2c__stm32_i2c_f4, NUMBER, i2c2_er_irq)
static_assert(I2C2_ERROR_IRQ == I2C2_ER_IRQn);

static inline void i2c2_enable_gpio_clocks(void) {
	__HAL_RCC_GPIOB_CLK_ENABLE();
}
static inline void i2c2_enable_i2c_clocks(void) {
	__HAL_RCC_I2C2_CLK_ENABLE();
}

#endif /* SRC_DRIVERS_ADAPTERS_STM32_I2C_CONF_F4_H_ */
