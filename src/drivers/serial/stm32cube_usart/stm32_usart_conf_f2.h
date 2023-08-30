/**
 * @file
 *
 * @date Aug 10, 2023
 * @author Anton Bondarev
 */

#ifndef SRC_DRIVERS_SERIAL_STM32CUBE_USART_STM32_USART_CONF_F2_H_
#define SRC_DRIVERS_SERIAL_STM32CUBE_USART_STM32_USART_CONF_F2_H_


#include "stm32f2xx_hal.h"
//#include "stm32f2xx_hal_usart.h"
//#include "stm32f2xx_hal_uart.h"

#include <assert.h>
#include <framework/mod/options.h>
#include <module/embox/driver/serial/stm_usart_f2.h>

#define MODOPS_USARTX OPTION_GET(NUMBER, usartx)

#define USART1_IRQ    \
	OPTION_MODULE_GET(embox__driver__serial__stm_usart_f2, NUMBER, usart1_irq)
static_assert(USART1_IRQ == USART1_IRQn, "");

#define USART2_IRQ    \
	OPTION_MODULE_GET(embox__driver__serial__stm_usart_f2, NUMBER, usart2_irq)
static_assert(USART2_IRQ == USART2_IRQn, "");

#define USART3_IRQ    \
	OPTION_MODULE_GET(embox__driver__serial__stm_usart_f2, NUMBER, usart3_irq)
static_assert(USART3_IRQ == USART3_IRQn, "");

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


#define STM32_USART_FLAGS(uart)   uart->SR
#define STM32_USART_RXDATA(uart)  uart->DR
#define STM32_USART_TXDATA(uart)  uart->DR

/* FIXME Reading in hasrx is weird */
#if 0
#define STM32_USART_CLEAR_ORE(uart) \
		do { \
			uint32_t __tmpreg; \
			if (STM32_USART_FLAGS(uart) & USART_SR_ORE) \
				/* Just read RX */ \
				__tmpreg = STM32_USART_RXDATA(uart); \
			(void) __tmpreg; \
		} while (0)
#else
#define STM32_USART_CLEAR_ORE(uart) \
		do { \
		} while (0)
#endif

#endif /* SRC_DRIVERS_SERIAL_STM32CUBE_USART_STM32_USART_CONF_F2_H_ */
