/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    06.08.2014
 */

#include <stdint.h>
#include <string.h>

#include <drivers/serial/diag_serial.h>

#include <drivers/serial/stm_usart.h>

#include <drivers/serial/uart_dev.h>

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

	if (dev->params.uart_param_flags & UART_PARAM_FLAGS_USE_IRQ) {
	    /* Enable the UART Data Register not empty Interrupt */
	    __HAL_UART_ENABLE_IT(&UartHandle, UART_IT_RXNE);
	}

	return 0;
}

static int stm32_uart_irq_en(struct uart *dev, const struct uart_params *params) {
	if (params->uart_param_flags & UART_PARAM_FLAGS_USE_IRQ) {
		UART_HandleTypeDef UartHandle;

		memset(&UartHandle, 0, sizeof(UartHandle));

		UartHandle.Instance = (void*) dev->base_addr;

		UartHandle.Init.BaudRate = params->baud_rate;
		UartHandle.Init.WordLength = UART_WORDLENGTH_8B;
		UartHandle.Init.StopBits = UART_STOPBITS_1;
		UartHandle.Init.Parity = UART_PARITY_NONE;
		UartHandle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
		UartHandle.Init.Mode = UART_MODE_TX_RX;

	    /* Enable the UART Data Register not empty Interrupt */
	    __HAL_UART_ENABLE_IT(&UartHandle, UART_IT_RXNE);
	}
	return 0;
}

static int stm32_uart_irq_dis(struct uart *dev, const struct uart_params *params) {
	if (params->uart_param_flags & UART_PARAM_FLAGS_USE_IRQ) {
		UART_HandleTypeDef UartHandle;

		memset(&UartHandle, 0, sizeof(UartHandle));

		UartHandle.Instance = (void*) dev->base_addr;

		UartHandle.Init.BaudRate = params->baud_rate;
		UartHandle.Init.WordLength = UART_WORDLENGTH_8B;
		UartHandle.Init.StopBits = UART_STOPBITS_1;
		UartHandle.Init.Parity = UART_PARITY_NONE;
		UartHandle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
		UartHandle.Init.Mode = UART_MODE_TX_RX;

		__HAL_UART_DISABLE_IT(&UartHandle, UART_IT_RXNE);
	}
	return 0;
}

const struct uart_ops stm32_uart_ops = {
		.uart_getc = stm32_uart_getc,
		.uart_putc = stm32_uart_putc,
		.uart_hasrx = stm32_uart_hasrx,
		.uart_setup = stm32_uart_setup,
		.uart_irq_en = stm32_uart_irq_en,
		.uart_irq_dis = stm32_uart_irq_dis,
};
