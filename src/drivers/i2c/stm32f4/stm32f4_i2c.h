/**
 * @file
 *
 * @data 29.03.2017
 * @author Alex Kalmuk
 */

#ifndef SRC_DRIVERS_I2C__STM32F4_STM32_USART_STM32F4_I2C_H_
#define SRC_DRIVERS_I2C__STM32F4_STM32_USART_STM32F4_I2C_H_

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_i2c.h"
#include "stm32f4_discovery.h"

extern int stm32f4_i2c_init(I2C_HandleTypeDef *I2cHandle, uint8_t slave_addr);

#endif /* SRC_DRIVERS_I2C__STM32F4_STM32_USART_STM32F4_I2C_H_ */
