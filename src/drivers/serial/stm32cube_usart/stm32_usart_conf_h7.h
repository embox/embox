/**
 * @file
 *
 * @data 30.11.2015
 * @author: Anton Bondarev
 */

#ifndef SRC_DRIVERS_SERIAL_STM32_USART_STM32_USART_CONF_H7_H_
#define SRC_DRIVERS_SERIAL_STM32_USART_STM32_USART_CONF_H7_H_

#include <bsp/stm32cube_hal.h>
#include <config/board_config.h>

#include <framework/mod/options.h>

#define MODOPS_USARTX OPTION_GET(NUMBER, usartx)


#if MODOPS_USARTX == 6

#define USARTx                           USART6
/* Definition for USARTx's NVIC */
#define USARTx_IRQn                      CONF_USART6_IRQ

#elif MODOPS_USARTX == 3
#define USARTx                           USART3

/* Definition for USARTx's NVIC */
#define USARTx_IRQn                      CONF_USART3_IRQ

#elif MODOPS_USARTX == 2
#define USARTx                           USART2


/* Definition for USARTx's NVIC */
#define USARTx_IRQn                      CONF_USART2_IRQ

#elif MODOPS_USARTX == 1
#define USARTx                           USART1

/* Definition for USARTx's NVIC */
#define USARTx_IRQn                      CONF_USART1_IRQ
#else
#error Unsupported USARTx
#endif


#define STM32_USART_FLAGS(uart)   uart->ISR
#define STM32_USART_RXDATA(uart)  uart->RDR
#define STM32_USART_TXDATA(uart)  uart->TDR
#define STM32_USART_ICR(uart)     uart->ICR

#define STM32_USART_CLEAR_ORE(uart) \
	do { \
		if (STM32_USART_FLAGS(uart) & USART_FLAG_ORE) \
			STM32_USART_ICR(uart) |= UART_CLEAR_OREF; \
	} while (0)

#endif /* SRC_DRIVERS_SERIAL_STM32_USART_STM32_USART_CONF_H7_H_ */
