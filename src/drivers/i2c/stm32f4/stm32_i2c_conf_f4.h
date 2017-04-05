/**
 * @file
 *
 * @data 29.03.2017
 * @author Alex Kalmuk
 */

#ifndef SRC_DRIVERS_I2C__STM32F4_STM32_USART_STM32_USART_CONF_F4_H_
#define SRC_DRIVERS_I2C__STM32F4_STM32_USART_STM32_USART_CONF_F4_H_

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_i2c.h"
#include "stm32f4_discovery.h"

#include <framework/mod/options.h>
#define MODOPS_I2CX OPTION_GET(NUMBER, i2cx)

#if MODOPS_I2CX == 1
#define I2Cx                             I2C1
#define I2Cx_CLK_ENABLE()                __HAL_RCC_I2C1_CLK_ENABLE()
#define I2Cx_SDA_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOB_CLK_ENABLE()
#define I2Cx_SCL_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOB_CLK_ENABLE()

#define I2Cx_FORCE_RESET()               __HAL_RCC_I2C1_FORCE_RESET()
#define I2Cx_RELEASE_RESET()             __HAL_RCC_I2C1_RELEASE_RESET()

/* Definition for I2Cx Pins */
#define I2Cx_SCL_PIN                    GPIO_PIN_6
#define I2Cx_SCL_GPIO_PORT              GPIOB
#define I2Cx_SCL_AF                     GPIO_AF4_I2C1
#define I2Cx_SDA_PIN                    GPIO_PIN_9
#define I2Cx_SDA_GPIO_PORT              GPIOB
#define I2Cx_SDA_AF                     GPIO_AF4_I2C1

#define I2Cx_EV_IRQn                    47
#define I2Cx_ER_IRQn                    48

#endif

#endif /* SRC_DRIVERS_I2C__STM32F4_STM32_USART_STM32_USART_CONF_F4_H_ */
