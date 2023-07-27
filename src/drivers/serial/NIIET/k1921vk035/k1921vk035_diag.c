#include <stdint.h>
#include <drivers/serial/uart_dev.h>
#include <drivers/serial/diag_serial.h>
#include <drivers/gpio/gpio.h>
#include <drivers/ttys.h>
#include "plib035_uart.h"

#define BAUDRATE OPTION_GET(NUMBER, baudrate)

extern const struct uart_ops k1921vk035_uart_ops;

static struct uart uart0 = {
		.uart_ops = &k1921vk035_uart_ops,
		// .irq_num = UART0_RX_IRQn, used for diag device, does not use interrupts
		.base_addr = (uint32_t)UART0,
        .params = {
            .baud_rate = BAUDRATE,
            .uart_param_flags = UART_PARAM_FLAGS_8BIT_WORD | UART_PARAM_FLAGS_DEV_TYPE_UART,
        },
};

DIAG_SERIAL_DEF(&uart0, &uart0.params);
