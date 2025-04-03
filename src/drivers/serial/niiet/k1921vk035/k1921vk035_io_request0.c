#include <drivers/serial/uart_dev.h>
#include <drivers/serial/diag_serial.h>
#include <drivers/gpio.h>
#include <drivers/ttys.h>
#include "errno.h"
#include "kernel/irq_lock.h"
#include "plib035_uart.h"
#include "semaphore.h"
#include "time.h"
#include "util/ring_buff.h"
#include <embox/unit.h>
#include <kernel/time/ktime.h>
#include <sys/uio.h>
#include <drivers/serial/k1921vk035_io_request.h>

#define BAUDRATE OPTION_GET(NUMBER, baudrate)
#define RX_BUFFER_SIZE OPTION_GET(NUMBER, rx_buffer_size)

#define UART0_RX_IRQn 26
#define UART0_E_RT_IRQn 28

extern const struct uart_ops k1921vk035_uart_ops;
extern irq_return_t uart_io_request_handler(unsigned int irq_nr, void *data);

RING_BUFFER_DEF(rx_ring_buff, char, RX_BUFFER_SIZE);

static struct uart uart_io_request0 = {
        .uart_ops = &k1921vk035_uart_ops,
        .irq_num = UART0_RX_IRQn,
        .base_addr = (uint32_t)UART0,
        .irq_handler = uart_io_request_handler,
        .params = {
            .baud_rate = BAUDRATE,
            .uart_param_flags = UART_PARAM_FLAGS_8BIT_WORD | UART_PARAM_FLAGS_DEV_TYPE_UART |
                UART_PARAM_FLAGS_USE_IRQ | UART_PARAM_FLAGS_PARITY_NONE,
        },
};

io_request_t io_request0 = {
    .type = IO_REQUEST_MODE_NONE,
    .mode = {},
    .rx_buff = &rx_ring_buff,
    .uart = &uart_io_request0,
};

EMBOX_UNIT_INIT(io_request0_init);

static int io_request0_init() {
    sem_init(&io_request0.semaphore, 0, 0);

    int retval = uart_register(&uart_io_request0, &uart_io_request0.params) || uart_open(&uart_io_request0);
    return retval;
}

STATIC_IRQ_ATTACH(UART0_RX_IRQn, uart_io_request_handler, &io_request0);
STATIC_IRQ_ATTACH(UART0_E_RT_IRQn, uart_io_request_handler, &io_request0);
