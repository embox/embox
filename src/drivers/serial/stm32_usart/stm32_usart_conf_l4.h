/**
 * @file stm32_usart_conf_l4.h
 * @brief Based on stm32_usart_conf_l3.h
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @date 20.11.2019
 */

#ifndef SRC_DRIVERS_SERIAL_STM32_USART_STM32_USART_CONF_L4_H_
#define SRC_DRIVERS_SERIAL_STM32_USART_STM32_USART_CONF_L4_H_

#define USE_HAL_UART_REGISTER_CALLBACKS 0
#define USE_HAL_USART_REGISTER_CALLBACKS 0

#include <stm32l475xx.h>
#include <stm32l4xx_hal_dma.h>
#include <stm32l4xx_hal_gpio.h>
#include <stm32l4xx_hal_rcc.h>
#include <stm32l4xx_hal_uart.h>
#include <stm32l4xx_hal_usart.h>

#include <framework/mod/options.h>
#define MODOPS_USARTX OPTION_GET(NUMBER, usartx)

#if MODOPS_USARTX == 1

#define USARTx                           USART1
#define USARTx_CLK_ENABLE()              __USART1_CLK_ENABLE();
#define USARTx_RX_GPIO_CLK_ENABLE()      __GPIOB_CLK_ENABLE()
#define USARTx_TX_GPIO_CLK_ENABLE()      __GPIOB_CLK_ENABLE()

#define USARTx_FORCE_RESET()             __USART1_FORCE_RESET()
#define USARTx_RELEASE_RESET()           __USART1_RELEASE_RESET()

/* Definition for USARTx Pins */
#define USARTx_TX_PIN                    GPIO_PIN_6
#define USARTx_TX_GPIO_PORT              GPIOB
#define USARTx_TX_AF                     GPIO_AF7_USART1
#define USARTx_RX_PIN                    GPIO_PIN_7
#define USARTx_RX_GPIO_PORT              GPIOB
#define USARTx_RX_AF                     GPIO_AF7_USART1

/* Definition for USARTx's NVIC */
/* In Embox we assume that the lower external irq number is 0,
 * but in the cortexm3 it is -15 */
#define USARTx_IRQn                      USART1_IRQn + 16
#define USARTx_IRQHandler                USART1_IRQHandler

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
