/**
 * @file
 *
 * @date Aug 10, 2023
 * @author Anton Bondarev
 */

#ifndef SRC_DRIVERS_SERIAL_STM32CUBE_USART_STM32_USART_CONF_F2_H_
#define SRC_DRIVERS_SERIAL_STM32CUBE_USART_STM32_USART_CONF_F2_H_

#include <bsp/stm32cube_hal.h>

#define STM32_USART_FLAGS(uart)   uart->SR
#define STM32_USART_RXDATA(uart)  uart->DR
#define STM32_USART_TXDATA(uart)  uart->DR

#define STM32_USART_CLEAR_ORE(uart) \
		do { \
		} while (0)


#endif /* SRC_DRIVERS_SERIAL_STM32CUBE_USART_STM32_USART_CONF_F2_H_ */
