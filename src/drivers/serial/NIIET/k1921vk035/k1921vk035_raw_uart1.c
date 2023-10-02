#include <stdint.h>
#include <embox/unit.h>
#include <drivers/serial/uart_dev.h>
#include "K1921VK035.h"

#define BAUDRATE OPTION_GET(NUMBER, baudrate)

extern const struct uart_ops k1921vk035_uart_ops;

static struct uart uart1 = {
		.uart_ops = &k1921vk035_uart_ops,
		.base_addr = (uint32_t)UART1,
        .params = {
            .baud_rate = BAUDRATE,
            .uart_param_flags = UART_PARAM_FLAGS_2_STOP | UART_PARAM_FLAGS_8BIT_WORD | UART_PARAM_FLAGS_DEV_TYPE_UART,
        },
};

EMBOX_UNIT_INIT(raw_uart1_init);

static int raw_uart1_init() {
    int retval = uart_register(&uart1, &uart1.params) || uart_open(&uart1);
    return retval;
}
