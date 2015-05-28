/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    06.08.2014
 */
/**
  ******************************************************************************
  * <h2><center>&copy; Portions COPYRIGHT 2012 Embest Tech. Co., Ltd.</center></h2>
  * @file    stm32f4_discovery.c
  * @author  CMP Team
  * @version V1.0.0
  * @date    28-December-2012
  * @brief   This file provides set of firmware functions to manage Leds and
  *          push-button available on STM32F4-Discovery Kit from STMicroelectronics.
  *          Modified to support the STM32F4DISCOVERY, STM32F4DIS-BB, STM32F4DIS-CAM
  *          and STM32F4DIS-LCD modules.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, Embest SHALL NOT BE HELD LIABLE FOR ANY DIRECT, INDIRECT
  * OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT
  * OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING INFORMATION
  * CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  ******************************************************************************
  */

#include <stdint.h>

#include <hal/reg.h>
#include <hal/system.h>
#include <drivers/diag.h>
#include <drivers/serial/diag.h>
#include <embox/unit.h>
#include <stm32f4xx_usart.h>
#include <stm32f4xx_rcc.h>
#include <stm32f4xx_gpio.h>

#include <drivers/serial/uart_device.h>

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
#define EVAL_COM_IRQn                   USART6_IRQn
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
#define EVAL_COM_IRQn                   USART2_IRQn
#else
#error Unsupported USARTx
#endif

static int stm32_uart_putc(struct uart *dev, int ch) {
	USART_TypeDef *USART = (void *) dev->base_addr;
	while (USART_GetFlagStatus(USART, USART_FLAG_TXE) == RESET);
	USART_SendData(USART, (uint8_t) ch);

	return 0;
}

static int stm32_uart_hasrx(struct uart *dev) {
	USART_TypeDef *USART = (void *) dev->base_addr;
	return USART_GetFlagStatus(USART, USART_FLAG_RXNE) == SET;
}

static int stm32_uart_getc(struct uart *dev) {
	USART_TypeDef *USART = (void *) dev->base_addr;
	return USART_ReceiveData(USART);
}

static void stm32_params2init(const struct uart_params *params, USART_InitTypeDef *USART_InitStruct) {
	/* TODO */
	USART_StructInit(USART_InitStruct);
	USART_InitStruct->USART_BaudRate = params->baud_rate;
}

/*
 *          1. Enable peripheral clock using the follwoing functions
 *             RCC_APB2PeriphClockCmd(RCC_APB2Periph_USARTx, ENABLE) for USART1 and USART6
 *             RCC_APB1PeriphClockCmd(RCC_APB1Periph_USARTx, ENABLE) for USART2, USART3, UART4 or UART5.
 *
 *          2.  According to the USART mode, enable the GPIO clocks using
 *              RCC_AHB1PeriphClockCmd() function. (The I/O can be TX, RX, CTS,
 *              or/and SCLK).
 *
 *          3. Peripheral's alternate function:
 *                 - Connect the pin to the desired peripherals' Alternate
 *                   Function (AF) using GPIO_PinAFConfig() function
 *                 - Configure the desired pin in alternate function by:
 *                   GPIO_InitStruct->GPIO_Mode = GPIO_Mode_AF
 *                 - Select the type, pull-up/pull-down and output speed via
 *                   GPIO_PuPd, GPIO_OType and GPIO_Speed members
 *                 - Call GPIO_Init() function
 *
 *          4. Program the Baud Rate, Word Length , Stop Bit, Parity, Hardware
 *             flow control and Mode(Receiver/Transmitter) using the USART_Init()
 *             function.
 *
 */
static int stm32_uart_setup(struct uart *dev, const struct uart_params *params) {
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStruct;

	/* Enable GPIO clock */
	RCC_AHB1PeriphClockCmd(EVAL_COM_TX_GPIO_CLK | EVAL_COM_RX_GPIO_CLK, ENABLE);

	/* Enable UART clock */
	EVAL_COM_CLK_CMD(EVAL_COM_CLK, ENABLE);

	/* Connect PXx to USARTx_Tx*/
	GPIO_PinAFConfig(EVAL_COM_TX_GPIO_PORT, EVAL_COM_TX_SOURCE, EVAL_COM_TX_AF);

	/* Connect PXx to USARTx_Rx*/
	GPIO_PinAFConfig(EVAL_COM_RX_GPIO_PORT, EVAL_COM_RX_SOURCE, EVAL_COM_RX_AF);

	/* Configure USART Tx as alternate function  */
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;

	GPIO_InitStructure.GPIO_Pin = EVAL_COM_TX_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(EVAL_COM_TX_GPIO_PORT, &GPIO_InitStructure);

	/* Configure USART Rx as alternate function  */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Pin = EVAL_COM_RX_PIN;
	GPIO_Init(EVAL_COM_RX_GPIO_PORT, &GPIO_InitStructure);

	stm32_params2init(params, &USART_InitStruct);
	USART_Init(EVAL_COM, &USART_InitStruct);
	USART_Cmd(EVAL_COM, ENABLE);

	return 0;
}

static const struct uart_ops stm32_uart_ops = {
		.uart_getc = stm32_uart_getc,
		.uart_putc = stm32_uart_putc,
		.uart_hasrx = stm32_uart_hasrx,
		.uart_setup = stm32_uart_setup,
};

static struct uart stm32_uart0 = {
		.uart_ops = &stm32_uart_ops,
		.irq_num = 0,
		.base_addr = (unsigned long) EVAL_COM,
};

static const struct uart_params uart_defparams = {
		.baud_rate = OPTION_GET(NUMBER,baud_rate),
		.parity = 0,
		.n_stop = 1,
		.n_bits = 8,
		.irq = false,
};

const struct uart_diag DIAG_IMPL_NAME(__EMBUILD_MOD__) = {
		.diag = {
			.ops = &uart_diag_ops,
		},
		.uart = &stm32_uart0,
		.params = &uart_defparams,
};
