/**
 * @file
 * @brief I2C driver for STM32
 *
 * @date    10.12.2018
 * @author  Alex Kalmuk
 */

#ifndef SRC_DRIVERS_ADAPTERS_I2C_STM32_I2C_H_
#define SRC_DRIVERS_ADAPTERS_I2C_STM32_I2C_H_

#include <drivers/i2c/stm32_i2c_conf.h>

struct stm32_i2c {
	I2C_HandleTypeDef *i2c_handle;
	I2C_TypeDef *i2c;
	int event_irq;
	int error_irq;
};

extern const struct i2c_algorithm stm32_i2c_algo;

extern int stm32_i2c_common_init(struct stm32_i2c *adapter);

#endif /* SRC_DRIVERS_ADAPTERS_I2C_STM32_I2C_H_ */
