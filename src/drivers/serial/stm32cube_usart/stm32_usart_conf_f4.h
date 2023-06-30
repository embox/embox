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

#include <assert.h>
#include <framework/mod/options.h>
#include <module/embox/driver/serial/stm_usart_f4.h>

#define MODOPS_USARTX OPTION_GET(NUMBER, usartx)

#define USART6_IRQ    \
	OPTION_MODULE_GET(embox__driver__serial__stm_usart_f4, NUMBER, usart6_irq)
static_assert(USART6_IRQ == USART6_IRQn, "");

#if !defined(STM32F411xE)
#define USART3_IRQ    \
	OPTION_MODULE_GET(embox__driver__serial__stm_usart_f4, NUMBER, usart3_irq)
static_assert(USART3_IRQ == USART3_IRQn, "");
#endif

#define USART2_IRQ    \
	OPTION_MODULE_GET(embox__driver__serial__stm_usart_f4, NUMBER, usart2_irq)
static_assert(USART2_IRQ == USART2_IRQn, "");

#define USART1_IRQ    \
	OPTION_MODULE_GET(embox__driver__serial__stm_usart_f4, NUMBER, usart1_irq)
static_assert(USART1_IRQ == USART1_IRQn, "");

#if defined(UART8)
#define UART8_IRQ    \
	OPTION_MODULE_GET(embox__driver__serial__stm_usart_f4, NUMBER, uart8_irq)
static_assert(UART8_IRQ == UART8_IRQn, "");
#endif

#if MODOPS_USARTX == 6

#define USARTx                           USART6

/* Definition for USARTx's NVIC */
#define USARTx_IRQn                      USART6_IRQ

#elif MODOPS_USARTX == 1
#define USARTx                           USART1

/* Definition for USARTx's NVIC */
#define USARTx_IRQn                      USART1_IRQ

#elif MODOPS_USARTX == 2
#define USARTx                           USART2

/* Definition for USARTx's NVIC */
#define USARTx_IRQn                      USART2_IRQ

#elif MODOPS_USARTX == 3
#define USARTx                           USART3

/* Definition for USARTx's NVIC */
#define USARTx_IRQn                      USART3_IRQ

#elif MODOPS_USARTX == 8
#if !defined(UART8)
#error Unsupported UART8 for this target
#endif

#define USARTx                           UART8

/* Definition for USARTx's NVIC */
#define USARTx_IRQn                      UART8_IRQ

#else
#error Unsupported USARTx
#endif

#define STM32_USART_FLAGS(uart)   uart->SR
#define STM32_USART_RXDATA(uart)  uart->DR
#define STM32_USART_TXDATA(uart)  uart->DR

/* Look at stm32f4xx_hal_uart.h and find ORE */
#define STM32_USART_CLEAR_ORE(uart) \
	do { \
		uint32_t __tmpreg; \
		if (STM32_USART_FLAGS(uart) & USART_SR_ORE) \
			/* Just read RX */ \
			__tmpreg = STM32_USART_RXDATA(uart); \
		(void) __tmpreg; \
	} while (0)

#endif /* SRC_DRIVERS_SERIAL_STM32_USART_STM32_USART_CONF_F4_H_ */
