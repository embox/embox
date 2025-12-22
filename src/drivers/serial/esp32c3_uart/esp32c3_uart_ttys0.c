#include <drivers/common/memory.h>
#include <drivers/serial/uart_dev.h>
#include <drivers/ttys.h>
#include <framework/mod/options.h>
#include <kernel/irq.h>

#define UART_BASE      UINTMAX_C(OPTION_GET(NUMBER, base_addr))
#define UART_BAUD_RATE OPTION_GET(NUMBER, baud_rate)

#define TTY_NAME  ttyS0

extern irq_return_t uart_irq_handler(unsigned int irq_nr, void *data);

extern const struct uart_ops esp32c3_uart_ops;

static struct uart esp32c3_uart_ttyS0 = {
    .uart_ops   = &esp32c3_uart_ops,
    .irq_num    = 0,
    .base_addr  = (unsigned long)UART_BASE,
    .params = ((struct uart_params){
        .baud_rate        = UART_BAUD_RATE,
        .uart_param_flags = UART_PARAM_FLAGS_8BIT_WORD,
    }),
};

PERIPH_MEMORY_DEFINE(esp32c3_uart, UART_BASE, 0x1000);

STATIC_IRQ_ATTACH(0, uart_irq_handler, &esp32c3_uart_ttyS0);

TTYS_DEF(TTY_NAME, &esp32c3_uart_ttyS0);