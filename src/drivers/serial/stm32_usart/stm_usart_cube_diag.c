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
#include <string.h>

#include <drivers/serial/diag_serial.h>

#include <drivers/serial/stm_usart.h>

#include <drivers/serial/uart_device.h>

static int stm32_uart_putc(struct uart *dev, int ch) {
	USART_TypeDef *uart = (void *) dev->base_addr;

	while ((STM32_USART_FLAGS(uart) & USART_FLAG_TXE) == 0);

	STM32_USART_TXDATA(uart) = (uint8_t) ch;

	return 0;
}

static int stm32_uart_hasrx(struct uart *dev) {
	USART_TypeDef *uart = (void *) dev->base_addr;

	/* Clear possible Overruns. It can happen, for example,
	 * when start scripts executed and you press buttons at the same time. */
	STM32_USART_CLEAR_ORE(uart);

	return STM32_USART_FLAGS(uart) & USART_FLAG_RXNE;
}

static int stm32_uart_getc(struct uart *dev) {
	USART_TypeDef *uart = (void *) dev->base_addr;

	return (uint8_t)(STM32_USART_RXDATA(uart) & 0xFF);
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
	UART_HandleTypeDef UartHandle;

	memset(&UartHandle, 0, sizeof(UartHandle));

	UartHandle.Instance = (void*) dev->base_addr;

	UartHandle.Init.BaudRate = params->baud_rate;
	UartHandle.Init.WordLength = UART_WORDLENGTH_8B;
	UartHandle.Init.StopBits = UART_STOPBITS_1;
	UartHandle.Init.Parity = UART_PARITY_NONE;
	UartHandle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	UartHandle.Init.Mode = UART_MODE_TX_RX;

	if (HAL_UART_Init(&UartHandle) != HAL_OK) {
		return -1;
	}

	if (dev->params.irq) {
	    /* Enable the UART Data Register not empty Interrupt */
	    __HAL_UART_ENABLE_IT(&UartHandle, UART_IT_RXNE);
	}

	return 0;
}

const struct uart_ops stm32_uart_ops = {
		.uart_getc = stm32_uart_getc,
		.uart_putc = stm32_uart_putc,
		.uart_hasrx = stm32_uart_hasrx,
		.uart_setup = stm32_uart_setup,
};

static struct uart stm32_diag = {
		.uart_ops = &stm32_uart_ops,
		.irq_num = USARTx_IRQn,
		.base_addr = (unsigned long) USARTx,
};

#if 0
static const struct uart_params uart_defparams = {
		.baud_rate = OPTION_GET(NUMBER,baud_rate),
		.parity = 0,
		.n_stop = 1,
		.n_bits = 8,
		.irq = true,
};
#endif
static const struct uart_params diag_defparams = {
		.baud_rate = OPTION_GET(NUMBER,baud_rate),
		.parity = 0,
		.n_stop = 1,
		.n_bits = 8,
		.irq = false,
};

DIAG_SERIAL_DEF(&stm32_diag, &diag_defparams);
