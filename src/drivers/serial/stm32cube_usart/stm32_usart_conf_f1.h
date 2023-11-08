/**
 * @file
 *
 * @date Sep 25, 2020
 * @author Anton Bondarev
 */

#ifndef SRC_DRIVERS_SERIAL_STM32_USART_STM32_USART_CONF_F1_H_
#define SRC_DRIVERS_SERIAL_STM32_USART_STM32_USART_CONF_F1_H_

#include <bsp/stm32cube_hal.h>


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

#endif /* SRC_DRIVERS_SERIAL_STM32_USART_STM32_USART_CONF_F1_H_ */
