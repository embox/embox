#include <drivers/serial/uart_dev.h>
#include <drivers/serial/diag_serial.h>
#include <drivers/gpio/gpio.h>
#include <drivers/ttys.h>
#include "plib035_uart.h"
#include "semaphore.h"
#include "util/ring_buff.h"
#include <embox/unit.h>

#define BAUDRATE OPTION_GET(NUMBER, baudrate)
#define RX_BUFFER_SIZE OPTION_GET(NUMBER, rx_buffer_size)

extern const struct uart_ops k1921vk035_uart_ops;

typedef enum {
    IO_REQUEST_NONE = 0,
    IO_REQUEST_READ = 1,
    IO_REQUEST_WRITE = 2,
} io_type;

RING_BUFFER_DEF(rx_ring_buff, char, RX_BUFFER_SIZE);

typedef struct {
    sem_t semaphore;
    io_type type;
    char* buffer;
    size_t count;
    struct ring_buff* rx_buff;
    struct uart* uart;
} io_request_t;

#include <kernel/printk.h>
void io_request_clean();
irq_return_t uart_io_request_handler(unsigned int irq_nr, void *data) {
    io_request_t* io_req = (io_request_t* )data;
    while(uart_hasrx(io_req->uart)) {
        irq_lock();
        int ch = uart_getc(io_req->uart);
        switch(io_req->type) {
            case IO_REQUEST_NONE:
            {
                /* store in intermediate buffer */
                int res = ring_buff_enqueue(io_req->rx_buff, &ch, 1);
                if(res == 0) {
                    printk("sKAJD:DJS\n");
                }
            } break;

            case IO_REQUEST_READ:
                io_req->buffer[0] = ch;
                io_req->buffer++;
                io_req->count--;
                if(io_req->count == 0) {
                    io_request_clean();
                    /* TODO: I'm not sure if this is safe todo in irq handler */
                    sem_post(&io_req->semaphore);
                }
                break;

            case IO_REQUEST_WRITE:
                break;
        }
    }
    irq_unlock();
    return IRQ_HANDLED;
}

static struct uart uart_io_request = {
		.uart_ops = &k1921vk035_uart_ops,
		.irq_num = UART0_RX_IRQn,
		.base_addr = (uint32_t)UART0,
        .irq_handler = uart_io_request_handler,
        .params = {
            .baud_rate = BAUDRATE,
            .uart_param_flags = UART_PARAM_FLAGS_8BIT_WORD | UART_PARAM_FLAGS_DEV_TYPE_UART | UART_PARAM_FLAGS_USE_IRQ,
        },
};

static io_request_t io_request = {
    .type = IO_REQUEST_NONE,
    .buffer = NULL,
    .count = 0,
    .uart = &uart_io_request,
};

void io_request_clean() {
    io_request.type = IO_REQUEST_NONE;
    io_request.buffer = NULL;
    io_request.count = 0;
}

int io_request_read(char* buf, size_t count) {
    irq_lock();
    {
        /* use buffer first */
        int ring_count = ring_buff_get_cnt(io_request.rx_buff);
        if(ring_count >= count) {
            /* we can satisfy call right now */
            for(int i = 0; i < count; i++) {
                ring_buff_dequeue(io_request.rx_buff, buf + i, 1);
            }
            irq_unlock();
            return count;
        } else {
            for(int i = 0; i < ring_count; i++) {
                ring_buff_dequeue(io_request.rx_buff, buf + i, 1);
            }
            io_request.type = IO_REQUEST_READ;
            io_request.buffer = buf + ring_count;
            io_request.count = count - ring_count;
        }
    }

    irq_unlock();
    // sched_wait_timeout(clock_t timeout, clock_t *remain);
    sem_wait(&io_request.semaphore);
    // sem_timedwait(&io_request.semaphore);
    return count;
}

EMBOX_UNIT_INIT(io_request_init);

static int io_request_init() {
    sem_init(&io_request.semaphore, 0, 0);
    io_request.type = IO_REQUEST_NONE;
    io_request.rx_buff = &rx_ring_buff;
    io_request.buffer = NULL;
    io_request.count = 0;
    io_request.uart = &uart_io_request;
    int retval = uart_register(&uart_io_request, &uart_io_request.params) || uart_open(&uart_io_request);
    return retval;
}

STATIC_IRQ_ATTACH(26, uart_io_request_handler, &io_request);
