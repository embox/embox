/**
 * @file
 *
 * @data 30.11.2015
 * @author: Anton Bondarev
 */

#ifndef SRC_DRIVERS_SERIAL_STM32_USART_STM32_USART_CONF_F4_H_
#define SRC_DRIVERS_SERIAL_STM32_USART_STM32_USART_CONF_F4_H_

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_usart.h"
#include "stm32f4xx_hal_uart.h"
#include "stm32f4_discovery.h"

#define MODOPS_USARTX OPTION_GET(NUMBER, usartx)

#define USARTx                           USART2
#define USARTx_CLK_ENABLE()              __HAL_RCC_USART2_CLK_ENABLE();
#define USARTx_RX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()
#define USARTx_TX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()

#define USARTx_FORCE_RESET()             __HAL_RCC_USART2_FORCE_RESET()
#define USARTx_RELEASE_RESET()           __HAL_RCC_USART2_RELEASE_RESET()

/* Definition for USARTx Pins */
#define USARTx_TX_PIN                    GPIO_PIN_2
#define USARTx_TX_GPIO_PORT              GPIOA
#define USARTx_TX_AF                     GPIO_AF7_USART2
#define USARTx_RX_PIN                    GPIO_PIN_3
#define USARTx_RX_GPIO_PORT              GPIOA
#define USARTx_RX_AF                     GPIO_AF7_USART2

/* Definition for USARTx's NVIC */
#define USARTx_IRQn                      USART2_IRQn
#define USARTx_IRQHandler                USART2_IRQHandler


#define STM32_USART_FLAGS(uart)   uart->SR
#define STM32_USART_RXDATA(uart)  uart->DR
#define STM32_USART_TXDATA(uart)  uart->DR


#endif /* SRC_DRIVERS_SERIAL_STM32_USART_STM32_USART_CONF_F4_H_ */
