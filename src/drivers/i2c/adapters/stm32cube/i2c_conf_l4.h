/**
 * @file
 *
 * @date    10.12.2018
 * @author  Alex Kalmuk
 */

#ifndef SRC_DRIVERS_ADAPTERS_STM32_I2C_CONF_L4_H_
#define SRC_DRIVERS_ADAPTERS_STM32_I2C_CONF_L4_H_

#include "stm32l4xx_hal.h"

#include <config/board_config.h>

/* FIXME from cube B-L475E-IOT01/stm32l475e_iot01.h */
#ifndef DISCOVERY_I2Cx_TIMING
#define DISCOVERY_I2Cx_TIMING                     ((uint32_t)0x00702681)
#endif /* DISCOVERY_I2Cx_TIMING */

#include <assert.h>
#include <framework/mod/options.h>
#include <module/embox/driver/i2c/stm32_i2c_l4.h>

#if 0
/* I2C1 */
#define I2C1_SCL_PIN  GPIO_PIN_8
#define I2C1_SDA_PIN  GPIO_PIN_9
#define I2C1_SCL_AF   GPIO_AF4_I2C1
#define I2C1_SDA_AF   GPIO_AF4_I2C1
#define I2C1_SCL_PORT GPIO_PORT_B
#define I2C1_SDA_PORT GPIO_PORT_B
#endif

#define I2C1_EVENT_IRQ  \
	OPTION_MODULE_GET(embox__driver__i2c__stm32_i2c_l4, NUMBER, i2c1_ev_irq)
static_assert(I2C1_EVENT_IRQ == I2C1_EV_IRQn, "");

#define I2C1_ERROR_IRQ \
	OPTION_MODULE_GET(embox__driver__i2c__stm32_i2c_l4, NUMBER, i2c1_er_irq)
static_assert(I2C1_ERROR_IRQ == I2C1_ER_IRQn, "");

#if 0
/* I2C2 */
#define I2C2_SCL_PIN  GPIO_PIN_10
#define I2C2_SDA_PIN  GPIO_PIN_11
#define I2C2_SCL_AF   GPIO_AF4_I2C2
#define I2C2_SDA_AF   GPIO_AF4_I2C2
#define I2C2_SCL_PORT GPIO_PORT_B
#define I2C2_SDA_PORT GPIO_PORT_B
#endif

#define I2C2_EVENT_IRQ  \
	OPTION_MODULE_GET(embox__driver__i2c__stm32_i2c_l4, NUMBER, i2c2_ev_irq)
static_assert(I2C2_EVENT_IRQ == I2C2_EV_IRQn, "");

#define I2C2_ERROR_IRQ  \
	OPTION_MODULE_GET(embox__driver__i2c__stm32_i2c_l4, NUMBER, i2c2_er_irq)
static_assert(I2C2_ERROR_IRQ == I2C2_ER_IRQn, "");

#endif /* SRC_DRIVERS_ADAPTERS_STM32_I2C_CONF_L4_H_ */
