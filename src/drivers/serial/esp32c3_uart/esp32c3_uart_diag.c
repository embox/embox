#include <drivers/common/memory.h>
#include <drivers/diag.h>
#include <drivers/serial/diag_serial.h>
#include <drivers/serial/uart_dev.h>
#include <framework/mod/options.h>

#define UART_BASE      UINTMAX_C(OPTION_GET(NUMBER, base_addr))
#define UART_BAUD_RATE OPTION_GET(NUMBER, baud_rate)

extern const struct uart_ops esp32c3_uart_ops;

static struct uart esp32c3_uart_diag = {
    .uart_ops   = &esp32c3_uart_ops,
    .irq_num    = 0,
    .base_addr  = (unsigned long) UART_BASE,
};

static const struct uart_params esp32c3_uart_diag_params = {
    .baud_rate        = UART_BAUD_RATE,
    .uart_param_flags = UART_PARAM_FLAGS_8BIT_WORD,
};

DIAG_SERIAL_DEF(&esp32c3_uart_diag, &esp32c3_uart_diag_params);

PERIPH_MEMORY_DEFINE(esp32c3_uart, UART_BASE, 0x1000);
