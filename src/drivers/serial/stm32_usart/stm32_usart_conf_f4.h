/**
 * @file
 *
 * @data 30.11.2015
 * @author: Anton Bondarev
 */

#ifndef SRC_DRIVERS_SERIAL_STM32_USART_STM32_USART_CONF_F4_H_
#define SRC_DRIVERS_SERIAL_STM32_USART_STM32_USART_CONF_F4_H_

#include <stm32f4xx_hal_dma.h>
#include <stm32f4xx_hal_usart.h>
#include <stm32f4xx_hal_rcc.h>
#include <stm32f4xx_hal_gpio.h>

#define MODOPS_USARTX OPTION_GET(NUMBER, usartx)

#if MODOPS_USARTX == 6
#define EVAL_COM                        USART6
#define EVAL_COM_CLK                    RCC_APB2Periph_USART6
#define EVAL_COM_CLK_CMD                RCC_APB2PeriphClockCmd
#define EVAL_COM_TX_PIN                 GPIO_Pin_6
#define EVAL_COM_TX_GPIO_PORT           GPIOC
#define EVAL_COM_TX_GPIO_CLK            RCC_AHB1Periph_GPIOC
#define EVAL_COM_TX_SOURCE              GPIO_PinSource6
#define EVAL_COM_TX_AF                  GPIO_AF_USART6
#define EVAL_COM_RX_PIN                 GPIO_Pin_7
#define EVAL_COM_RX_GPIO_PORT           GPIOC
#define EVAL_COM_RX_GPIO_CLK            RCC_AHB1Periph_GPIOC
#define EVAL_COM_RX_SOURCE              GPIO_PinSource7
#define EVAL_COM_RX_AF                  GPIO_AF_USART6
/* In Embox we assume that the lower external irq number is 0,
 * but in the cortexm3 it is -15 */
#define EVAL_COM_IRQn                   (USART6_IRQn + 16)
#elif MODOPS_USARTX == 2
#define EVAL_COM                        USART2
#define EVAL_COM_CLK                    RCC_APB1Periph_USART2
#define EVAL_COM_CLK_CMD                RCC_APB1PeriphClockCmd
#define EVAL_COM_TX_PIN                 GPIO_Pin_5
#define EVAL_COM_TX_GPIO_PORT           GPIOD
#define EVAL_COM_TX_GPIO_CLK            RCC_AHB1Periph_GPIOD
#define EVAL_COM_TX_SOURCE              GPIO_PinSource5
#define EVAL_COM_TX_AF                  GPIO_AF_USART2
#define EVAL_COM_RX_PIN                 GPIO_Pin_6
#define EVAL_COM_RX_GPIO_PORT           GPIOD
#define EVAL_COM_RX_GPIO_CLK            RCC_AHB1Periph_GPIOD
#define EVAL_COM_RX_SOURCE              GPIO_PinSource6
#define EVAL_COM_RX_AF                  GPIO_AF_USART2
#define EVAL_COM_IRQn                   (USART2_IRQn + 16)
#else
#error Unsupported USARTx
#endif


#endif /* SRC_DRIVERS_SERIAL_STM32_USART_STM32_USART_CONF_F4_H_ */
