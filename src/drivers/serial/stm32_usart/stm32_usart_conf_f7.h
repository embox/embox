/**
 * @file
 *
 * @data 30.11.2015
 * @author: Anton Bondarev
 */

#ifndef SRC_DRIVERS_SERIAL_STM32_USART_STM32_USART_CONF_F7_H_
#define SRC_DRIVERS_SERIAL_STM32_USART_STM32_USART_CONF_F7_H_

#include "stm32f7xx_hal.h"
#include "stm32f7xx_hal_usart.h"
#include "stm32f7xx_hal_uart.h"

#if defined STM32F746xx
#include "stm32746g_discovery.h"
#elif defined STM32F769xx
#include "stm32f769i_discovery.h"
#else
#error Unsupported platform
#endif

#include <assert.h>
#include <framework/mod/options.h>
#include <module/embox/driver/serial/stm_usart_f7.h>

#define MODOPS_USARTX OPTION_GET(NUMBER, usartx)

#define USART6_IRQ    \
	OPTION_MODULE_GET(embox__driver__serial__stm_usart_f7, NUMBER, usart6_irq)
static_assert(USART6_IRQ == USART6_IRQn, "");

#define USART2_IRQ    \
	OPTION_MODULE_GET(embox__driver__serial__stm_usart_f7, NUMBER, usart2_irq)
static_assert(USART2_IRQ == USART2_IRQn, "");

#define USART1_IRQ    \
	OPTION_MODULE_GET(embox__driver__serial__stm_usart_f7, NUMBER, usart1_irq)
static_assert(USART1_IRQ == USART1_IRQn, "");

#if MODOPS_USARTX == 6

#define USARTx                           USART6
/* Definition for USARTx's NVIC */
#define USARTx_IRQn                      USART6_IRQ

#elif MODOPS_USARTX == 2
#define USARTx                           USART2


/* Definition for USARTx's NVIC */
#define USARTx_IRQn                      USART2_IRQ

#elif MODOPS_USARTX == 1
#define USARTx                           USART1

/* Definition for USARTx's NVIC */
#define USARTx_IRQn                      USART1_IRQ
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

#endif /* SRC_DRIVERS_SERIAL_STM32_USART_STM32_USART_CONF_F7_H_ */
