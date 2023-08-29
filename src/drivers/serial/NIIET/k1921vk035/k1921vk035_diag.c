#include <stdint.h>
#include <drivers/serial/uart_dev.h>
#include <drivers/serial/diag_serial.h>
#include <drivers/gpio/gpio.h>
#include <drivers/ttys.h>
#include "plib035_uart.h"

#define UART_NUM OPTION_GET(NUMBER, uart_num)
#define BAUDRATE OPTION_GET(NUMBER, baudrate)

#define CONCAT(a, b) a ## b
#define CONCAT_EXPANDED(a, b) CONCAT(a, b)

#define UART_NUM_TO_UART(uart_num) CONCAT_EXPANDED(UART,uart_num) // Example: UART_NUM_TO_UART(1) -> UART1
#define DIAG_UART UART_NUM_TO_UART(UART_NUM) 


extern const struct uart_ops k1921vk035_uart_ops;

static struct uart uart_diag = {
		.uart_ops = &k1921vk035_uart_ops,
		.base_addr = (uint32_t)DIAG_UART,
        .params = {
            .baud_rate = BAUDRATE,
            .uart_param_flags = UART_PARAM_FLAGS_8BIT_WORD | UART_PARAM_FLAGS_DEV_TYPE_UART,
        },
};

DIAG_SERIAL_DEF(&uart_diag, &uart_diag.params);
