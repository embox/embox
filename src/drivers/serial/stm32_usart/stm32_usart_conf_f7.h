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
static_assert(USART6_IRQ == USART6_IRQn);

#define USART2_IRQ    \
	OPTION_MODULE_GET(embox__driver__serial__stm_usart_f7, NUMBER, usart2_irq)
static_assert(USART2_IRQ == USART2_IRQn);

#define USART1_IRQ    \
	OPTION_MODULE_GET(embox__driver__serial__stm_usart_f7, NUMBER, usart1_irq)
static_assert(USART1_IRQ == USART1_IRQn);

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

#define STM32_USART1_ENABLED             1
#define USART1_TX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()
#define USART1_TX_PIN                    GPIO_PIN_9
#define USART1_TX_GPIO_PORT              GPIOA
#define USART1_TX_AF                     GPIO_AF7_USART1
#if defined STM32F746xx
#define USART1_RX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOB_CLK_ENABLE()
#define USART1_RX_PIN                    GPIO_PIN_7
#define USART1_RX_GPIO_PORT              GPIOB
#define USART1_RX_AF                     GPIO_AF7_USART1
#elif defined STM32F769xx
#define USART1_RX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()
#define USART1_RX_PIN                    GPIO_PIN_10
#define USART1_RX_GPIO_PORT              GPIOA
#define USART1_RX_AF                     GPIO_AF7_USART1
#else
#error Unsupported platform
#endif

#define STM32_USART2_ENABLED             1
#define USART2_RX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()
#define USART2_TX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()
#define USART2_TX_PIN                    GPIO_PIN_2
#define USART2_TX_GPIO_PORT              GPIOA
#define USART2_TX_AF                     GPIO_AF7_USART2
#define USART2_RX_PIN                    GPIO_PIN_3
#define USART2_RX_GPIO_PORT              GPIOA
#define USART2_RX_AF                     GPIO_AF7_USART2

#define STM32_USART6_ENABLED             1
#define USART6_RX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOC_CLK_ENABLE()
#define USART6_TX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOC_CLK_ENABLE()
#define USART6_TX_PIN                    GPIO_PIN_6
#define USART6_TX_GPIO_PORT              GPIOC
#define USART6_TX_AF                     GPIO_AF8_USART6
#define USART6_RX_PIN                    GPIO_PIN_7
#define USART6_RX_GPIO_PORT              GPIOC
#define USART6_RX_AF                     GPIO_AF8_USART6



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
