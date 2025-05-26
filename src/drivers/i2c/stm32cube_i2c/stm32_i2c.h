/**
 * @file
 * @brief I2C driver for STM32
 *
 * @date    10.12.2018
 * @author  Alex Kalmuk
 */

#ifndef SRC_DRIVERS_ADAPTERS_I2C_STM32_I2C_H_
#define SRC_DRIVERS_ADAPTERS_I2C_STM32_I2C_H_

#include <bsp/stm32cube_hal.h>
#include <drivers/i2c/i2c.h>
#include <framework/mod/options.h>

#include <config/embox/driver/i2c/stm32cube_i2c.h>

#define USE_I2C_IRQ \
	OPTION_MODULE_GET(embox__driver__i2c__stm32cube_i2c, BOOLEAN, use_i2c_irq)

struct stm32_i2c_priv {
	void (*init_pins)(void);
	I2C_HandleTypeDef *i2c_handle;
	I2C_TypeDef *i2c;
	int event_irq;
	int error_irq;
};

extern const struct i2c_ops stm32_i2c_ops;

extern int stm32_i2c_common_init(struct stm32_i2c_priv *adapter);

#endif /* SRC_DRIVERS_ADAPTERS_I2C_STM32_I2C_H_ */
