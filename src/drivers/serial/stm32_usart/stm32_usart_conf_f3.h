/**
 * @file
 *
 * @data 03.12.2015
 * @author: Anton Bondarev
 */

#ifndef SRC_DRIVERS_SERIAL_STM32_USART_STM32_USART_CONF_F3_H_
#define SRC_DRIVERS_SERIAL_STM32_USART_STM32_USART_CONF_F3_H_

/* Includes ------------------------------------------------------------------*/
#include "stm32f3xx_hal.h"
#include "stm32f3_discovery.h"

//http://myprognote.blogspot.ru/2013/03/usart-stm32f3.html

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* User can use this section to tailor USARTx/UARTx instance used and associated
   resources */
/* Definition for USARTx clock resources */

#include <assert.h>
#include <framework/mod/options.h>
#include <module/embox/driver/serial/stm_usart_f3.h>

#define MODOPS_USARTX OPTION_GET(NUMBER, usartx)

#define USART1_IRQ    \
	OPTION_MODULE_GET(embox__driver__serial__stm_usart_f3, NUMBER, usart1_irq)
static_assert(USART1_IRQ == USART1_IRQn);

#define USART2_IRQ    \
	OPTION_MODULE_GET(embox__driver__serial__stm_usart_f3, NUMBER, usart2_irq)
static_assert(USART2_IRQ == USART2_IRQn);

#define USART3_IRQ    \
	OPTION_MODULE_GET(embox__driver__serial__stm_usart_f3, NUMBER, usart3_irq)
static_assert(USART3_IRQ == USART3_IRQn);

#if MODOPS_USARTX == 1

#define USARTx                           USART1

#define USARTx_IRQn                      USART1_IRQ

#elif MODOPS_USARTX == 2

#define USARTx                           USART2
#define USARTx_IRQn                      USART2_IRQ

#elif MODOPS_USARTX == 3

#define USARTx                           USART3

#define USARTx_IRQn                      USART3_IRQ

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

#endif /* SRC_DRIVERS_SERIAL_STM32_USART_STM32_USART_CONF_F3_H_ */
