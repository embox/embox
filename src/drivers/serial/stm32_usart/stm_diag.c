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

extern const struct uart_ops stm32_uart_ops;

static struct uart stm32_diag = {
		.uart_ops = &stm32_uart_ops,
		.irq_num = USARTx_IRQn,
		.base_addr = (unsigned long) USARTx,
};

static const struct uart_params diag_defparams = {
		.baud_rate = OPTION_GET(NUMBER,baud_rate),
		.uart_param_flags = UART_PARAM_FLAGS_8BIT_WORD,
};

DIAG_SERIAL_DEF(&stm32_diag, &diag_defparams);
