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

static void stm32cube_fill_uart_init_param(const struct uart_params *params, UART_InitTypeDef *init) {
	uint32_t flags;
	
	flags = params->uart_param_flags;

	switch (UART_PARAM_FLAGS_PARITY_MASK(flags)) {
		case UART_PARAM_FLAGS_PARITY_ODD:
			init->Parity = UART_PARITY_ODD;
			break;
		case UART_PARAM_FLAGS_PARITY_EVEN:
			init->Parity = UART_PARITY_EVEN;
			break;
		case UART_PARAM_FLAGS_PARITY_NONE:
		default:
			init->Parity = UART_PARITY_NONE;
			break;
	}

	switch (UART_PARAM_FLAGS_BIT_WORD_MASK(flags)) {
#if defined (UART_WORDLENGTH_7B)
		case UART_PARAM_FLAGS_7BIT_WORD:
			init->WordLength = UART_WORDLENGTH_7B;
			break;
#endif
		case UART_PARAM_FLAGS_9BIT_WORD:
			init->WordLength = UART_WORDLENGTH_9B;
			break;
		case UART_PARAM_FLAGS_8BIT_WORD:
		default:
			init->WordLength = UART_WORDLENGTH_8B;
			break;
	}


	switch (UART_PARAM_FLAGS_STOPS_MASK(flags)) {
#if defined (UART_STOPBITS_0_5)
		case UART_PARAM_FLAGS_0_5_STOP:
			init->StopBits = UART_STOPBITS_0_5;
			break;
#endif
#if defined (UART_STOPBITS_1_5)
		case UART_PARAM_FLAGS_1_5_STOP:
			init->StopBits = UART_STOPBITS_1_5;
			break;
#endif
		case UART_PARAM_FLAGS_2_STOP:
			init->StopBits = UART_STOPBITS_2;
			break;
		case UART_PARAM_FLAGS_1_STOP:
		default:
			init->StopBits = UART_STOPBITS_1;
			break;
	}

	switch (UART_PARAM_FLAGS_HWCTRL_MASK(flags)) {
		case UART_PARAM_FLAGS_HWCRTL_CTS:
			init->HwFlowCtl = UART_HWCONTROL_CTS;
			break;
		case UART_PARAM_FLAGS_HWCRTL_RTS:
			init->HwFlowCtl = UART_HWCONTROL_RTS;
			break;
		case UART_PARAM_FLAGS_HWCRTL_BOTH:
			init->HwFlowCtl = UART_HWCONTROL_RTS_CTS;
			break;
		case UART_PARAM_FLAGS_HWCRTL_NONE:
		default:
			init->HwFlowCtl = UART_HWCONTROL_NONE;
			break;
	}

	init->BaudRate = params->baud_rate;
	init->Mode = UART_MODE_TX_RX;

}

static int stm32_uart_setup(struct uart *dev, const struct uart_params *params) {
	UART_HandleTypeDef UartHandle;

	memset(&UartHandle, 0, sizeof(UartHandle));

	UartHandle.Instance = (void*) dev->base_addr;

	stm32cube_fill_uart_init_param(params, &UartHandle.Init);

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

		stm32cube_fill_uart_init_param(params, &UartHandle.Init);
	
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

		stm32cube_fill_uart_init_param(params, &UartHandle.Init);

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
