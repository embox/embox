/**
 * @file stm32_usart_conf_l4.h
 * @brief Based on stm32_usart_conf_l3.h
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @date 20.11.2019
 */

#ifndef SRC_DRIVERS_SERIAL_STM32_USART_STM32_USART_CONF_L4_H_
#define SRC_DRIVERS_SERIAL_STM32_USART_STM32_USART_CONF_L4_H_

#include <bsp/stm32cube_hal.h>
#include <config/board_config.h>

#include <framework/mod/options.h>

#define MODOPS_USARTX OPTION_GET(NUMBER, usartx)

#if MODOPS_USARTX == 1

#define USARTx                           USART1

#define USARTx_IRQn                      CONF_USART1_IRQ

#elif MODOPS_USARTX == 2

#define USARTx                           USART2

#define USARTx_IRQn                      CONF_USART2_IRQ

#else
#error "Unsupported USART number"
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

#endif /* SRC_DRIVERS_SERIAL_STM32_USART_STM32_USART_CONF_L4_H_ */
