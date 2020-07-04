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

#include <stm32l4xx_hal_dma.h>
#include <stm32l4xx_hal_gpio.h>
#include <stm32l4xx_hal_rcc.h>
#include <stm32l4xx_hal_uart.h>
#include <stm32l4xx_hal_usart.h>

#include <assert.h>
#include <framework/mod/options.h>
#include <module/embox/driver/serial/stm_usart_l4.h>

#define MODOPS_USARTX OPTION_GET(NUMBER, usartx)

#define USART1_IRQ    \
	OPTION_MODULE_GET(embox__driver__serial__stm_usart_l4, NUMBER, usart1_irq)
static_assert(USART1_IRQ == USART1_IRQn);

#define USART2_IRQ    \
	OPTION_MODULE_GET(embox__driver__serial__stm_usart_l4, NUMBER, usart2_irq)
static_assert(USART2_IRQ == USART2_IRQn);

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

#define USARTx_IRQn                      USART1_IRQ
#define USARTx_IRQHandler                USART1_IRQHandler

#elif MODOPS_USARTX == 2

#define USARTx                           USART2
#define USARTx_CLK_ENABLE()              __USART2_CLK_ENABLE();
#define USARTx_RX_GPIO_CLK_ENABLE()      __GPIOA_CLK_ENABLE()
#define USARTx_TX_GPIO_CLK_ENABLE()      __GPIOA_CLK_ENABLE()

#define USARTx_FORCE_RESET()             __USART2_FORCE_RESET()
#define USARTx_RELEASE_RESET()           __USART2_RELEASE_RESET()

/* Definition for USARTx Pins */
#define USARTx_TX_PIN                    GPIO_PIN_2
#define USARTx_TX_GPIO_PORT              GPIOA
#define USARTx_TX_AF                     GPIO_AF7_USART2
#define USARTx_RX_PIN                    GPIO_PIN_3
#define USARTx_RX_GPIO_PORT              GPIOA
#define USARTx_RX_AF                     GPIO_AF7_USART2

#define USARTx_IRQn                      USART2_IRQ
#define USARTx_IRQHandler                USART2_IRQHandler

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
