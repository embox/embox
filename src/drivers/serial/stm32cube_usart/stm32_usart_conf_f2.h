/**
 * @file
 *
 * @date Aug 10, 2023
 * @author Anton Bondarev
 */

#ifndef SRC_DRIVERS_SERIAL_STM32CUBE_USART_STM32_USART_CONF_F2_H_
#define SRC_DRIVERS_SERIAL_STM32CUBE_USART_STM32_USART_CONF_F2_H_

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

#elif MODOPS_USARTX == 3

#define USARTx                           USART3

#define USARTx_IRQn                      CONF_USART3_IRQ

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
