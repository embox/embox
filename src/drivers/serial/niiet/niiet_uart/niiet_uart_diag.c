/**
 * @file
 * @brief
 *
 * @author  Andrew Bursian
 * @date    20.02.2023
 */

#include <stdint.h>
#include <hal/reg.h>
#include <drivers/diag.h>
#include <drivers/serial/uart_dev.h>
#include <drivers/serial/diag_serial.h>


#include <framework/mod/options.h>

#define UART_BASE OPTION_GET(NUMBER, base_addr)
#define IRQ_NUM   OPTION_GET(NUMBER, irq_num)
#define BAUD_RATE OPTION_GET(NUMBER, baud_rate)

extern const struct uart_ops niiet_uart_ops;

static struct uart uart0 = {
		.uart_ops = &niiet_uart_ops,
		.irq_num = IRQ_NUM,
		.base_addr = UART_BASE,
};

static const struct uart_params uart_diag_params = {
		.baud_rate = BAUD_RATE,
		.uart_param_flags = UART_PARAM_FLAGS_8BIT_WORD | UART_PARAM_FLAGS_DEV_TYPE_UART
};

DIAG_SERIAL_DEF(&uart0, &uart_diag_params);

