/**
 * @file
 * @brief
 * @author Andrew Bursian
 * @date 28.03.2024
 */

#ifndef SRC_DRIVERS_SERIAL_STM32CUBE_USART_STM32_USART_CONF_L1_H_
#define SRC_DRIVERS_SERIAL_STM32CUBE_USART_STM32_USART_CONF_L1_H_

#include <bsp/stm32cube_hal.h>

#define STM32_USART_FLAGS(uart)   uart->SR
#define STM32_USART_RXDATA(uart)  uart->DR
#define STM32_USART_TXDATA(uart)  uart->DR

#define STM32_USART_CLEAR_ORE(uart) \
		do { \
		} while (0)


#endif /* SRC_DRIVERS_SERIAL_STM32CUBE_USART_STM32_USART_CONF_L1_H_ */
