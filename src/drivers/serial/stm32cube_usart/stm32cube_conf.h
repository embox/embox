/**
 * @file
 *
 * @date 30.11.2015
 * @author: Anton Bondarev
 */

#ifndef SRC_DRIVERS_SERIAL_STM32_USART_STM32CUBE_CONF_H_
#define SRC_DRIVERS_SERIAL_STM32_USART_STM32CUBE_CONF_H_

#include <bsp/stm32cube_hal.h>

#include <config/board_config.h>

#include <framework/mod/options.h>

#define MODOPS_USARTX OPTION_GET(NUMBER, usartx)


#if MODOPS_USARTX == 6

#define USARTx                           USART6

/* Definition for USARTx's NVIC */
#define USARTx_IRQn                      CONF_USART6_IRQ

#elif MODOPS_USARTX == 1
#define USARTx                           USART1

/* Definition for USARTx's NVIC */
#define USARTx_IRQn                      CONF_USART1_IRQ

#elif MODOPS_USARTX == 2
#define USARTx                           USART2

/* Definition for USARTx's NVIC */
#define USARTx_IRQn                      CONF_USART2_IRQ

#elif MODOPS_USARTX == 3
#define USARTx                           USART3

/* Definition for USARTx's NVIC */
#define USARTx_IRQn                      CONF_USART3_IRQ

#elif MODOPS_USARTX == 8
#if !defined(UART8)
#error Unsupported UART8 for this target
#endif

#define USARTx                           UART8

/* Definition for USARTx's NVIC */
#define USARTx_IRQn                      CONF_UART8_IRQ

#else
#error Unsupported USARTx
#endif

#endif /* SRC_DRIVERS_SERIAL_STM32_USART_STM32CUBE_CONF_H_ */
