/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 11.02.25
 */

#include <drivers/common/memory.h>
#include <drivers/serial/uart_dev.h>
#include <drivers/ttys.h>
#include <framework/mod/options.h>

#define TTY_NAME  spi2uart
#define BAUD_RATE OPTION_GET(NUMBER, baud_rate)

extern const struct uart_ops xr20m1172_uart_ops;

static struct uart xr20m1172_uart = {
    .uart_ops = &xr20m1172_uart_ops,
    .params = ((struct uart_params){
        .baud_rate = BAUD_RATE,
        .uart_param_flags = UART_PARAM_FLAGS_8BIT_WORD,
    }),
};

TTYS_DEF(TTY_NAME, &xr20m1172_uart);
