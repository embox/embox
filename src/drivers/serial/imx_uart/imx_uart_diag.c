/**
 * @brief
 *
 * @date 14.12.23
 * @author Aleksey Zhmulin
 */
#include <drivers/common/memory.h>
#include <drivers/serial/diag_serial.h>
#include <drivers/serial/uart_dev.h>
#include <framework/mod/options.h>

#define BASE_ADDR OPTION_GET(NUMBER, base_addr)
#define BAUD_RATE OPTION_GET(NUMBER, baud_rate)

extern const struct uart_ops imx_uart_ops;

static struct uart imx_uart_diag = {
    .uart_ops = &imx_uart_ops,
    .base_addr = BASE_ADDR,
};

static const struct uart_params imx_uart_diag_params = {
    .baud_rate = BAUD_RATE,
    .uart_param_flags = UART_PARAM_FLAGS_8BIT_WORD,
};

DIAG_SERIAL_DEF(&imx_uart_diag, &imx_uart_diag_params);

PERIPH_MEMORY_DEFINE(imx_uart_diag, BASE_ADDR, 0xc0);
