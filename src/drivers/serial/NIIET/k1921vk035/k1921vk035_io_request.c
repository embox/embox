#include <drivers/serial/uart_dev.h>
#include <drivers/serial/diag_serial.h>
#include <drivers/gpio/gpio.h>
#include <drivers/ttys.h>
#include "errno.h"
#include "kernel/irq_lock.h"
#include "plib035_uart.h"
#include "semaphore.h"
#include "time.h"
#include "util/ring_buff.h"
#include <embox/unit.h>
#include <kernel/time/ktime.h>

#define BAUDRATE OPTION_GET(NUMBER, baudrate)
#define RX_BUFFER_SIZE OPTION_GET(NUMBER, rx_buffer_size)

#define UART0_RX_IRQn 26
#define UART0_E_RT_IRQn 28

extern const struct uart_ops k1921vk035_uart_ops;

typedef enum {
    IO_REQUEST_NONE = 0,
    IO_REQUEST_READ = 1,
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
                    /* TODO: no one wants to read out data so intermediate buffer is overflowed, what do we do? */
                }
            } break;

            case IO_REQUEST_READ:
            {
                io_req->buffer[0] = ch;
                io_req->buffer++;
                io_req->count--;
                if(io_req->count == 0) {
                    io_request_clean();
                    sem_post(&io_req->semaphore);
                }
            } break;

        }
    }
    irq_unlock();
    return IRQ_HANDLED;
}

static struct uart uart_io_request = {
        .uart_ops = NULL,  // set at runtime
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
    sem_wait(&io_request.semaphore);
    return count;
}

int io_request_read_timeout(char* buf, size_t count, int32_t timeout_ms) {
    irq_lock();
    int ring_count = ring_buff_get_cnt(io_request.rx_buff);
    int read_count = ring_count > count ? count : ring_count;
    if(timeout_ms < 0) {
        /* No timeout, flush all we got */
        for(int i = 0; i < read_count; i++) {
            ring_buff_dequeue(io_request.rx_buff, buf + i, 1);
        }
        irq_unlock();
        return read_count;
    } else {
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
    struct timespec current_time = {};
    clock_gettime(CLOCK_REALTIME, &current_time);
    struct timespec wait_time = timespec_add(current_time, ns_to_timespec((uint64_t)(timeout_ms) * 1000000));\
    int wait_res = sem_timedwait(&io_request.semaphore, &wait_time);
    if(wait_res == -ETIMEDOUT) {
        io_request_clean();
        /* TODO: hack - we should calculate how much we actially read */
        return count - 1;
    }
    return count;
}

#define UART_GPIO			GPIO_PORT_B
#define UART0_GPIO_TX_mask		(1<<10)
#define UART0_GPIO_RX_mask		(1<<11)
#define UART1_GPIO_TX_mask		(1<<8)
#define UART1_GPIO_RX_mask		(1<<9)

static int k1921vk035_uart_setup_io_request(struct uart *dev, const struct uart_params *params) {
    UART_TypeDef* uart = (void* )dev->base_addr;
    UART_Num_TypeDef uart_num;
    gpio_mask_t uart_gpio_mask;
    if (uart == UART0) {
        uart_num = UART0_Num;
        uart_gpio_mask = UART0_GPIO_TX_mask | UART0_GPIO_RX_mask;
    } else {
        uart_num = UART1_Num;
        uart_gpio_mask = UART1_GPIO_TX_mask | UART1_GPIO_RX_mask;
    }

    RCU_UARTClkConfig(uart_num, RCU_PeriphClk_PLLClk, 0, DISABLE);
    RCU_UARTClkCmd(uart_num, ENABLE);
    RCU_UARTRstCmd(uart_num, ENABLE);

    // This also enables gpio port
    gpio_setup_mode(GPIO_PORT_B, uart_gpio_mask, GPIO_MODE_OUT_ALTERNATE);

    UART_Init_TypeDef uart_init_struct;
    UART_StructInit(&uart_init_struct);

    uart_init_struct.BaudRate = params->baud_rate;

    uart_init_struct.DataWidth = UART_DataWidth_8;
    uart_init_struct.FIFO = ENABLE;
    uart_init_struct.ParityBit = UART_ParityBit_Disable;

    uart_init_struct.StopBit =
        (params->uart_param_flags & UART_PARAM_FLAGS_2_STOP) ? UART_StopBit_2 : UART_StopBit_1;
    uart_init_struct.Rx = ENABLE;
    uart_init_struct.Tx = ENABLE;

    NVIC_EnableIRQ(UART0_E_RT_IRQn);
    NVIC_SetPriority(UART0_E_RT_IRQn, 0);

    UART_Init(uart, &uart_init_struct);
    UART_Cmd(uart, ENABLE);
    if(params->uart_param_flags & UART_PARAM_FLAGS_USE_IRQ) {
        UART_ITFIFOLevelRxConfig(uart, UART_FIFOLevel_1_8);
        UART_ITCmd(uart, UART_ITSource_RecieveTimeout | UART_ITSource_RxFIFOLevel, ENABLE);
    }

    return 0;
}

struct uart_ops k1921vk035_uart_ops_io_request;

EMBOX_UNIT_INIT(io_request_init);

static int io_request_init() {
    sem_init(&io_request.semaphore, 0, 0);
    io_request.type = IO_REQUEST_NONE;
    io_request.rx_buff = &rx_ring_buff;
    io_request.buffer = NULL;
    io_request.count = 0;
    io_request.uart = &uart_io_request;

    k1921vk035_uart_ops_io_request = k1921vk035_uart_ops;
    k1921vk035_uart_ops_io_request.uart_setup = k1921vk035_uart_setup_io_request;

    uart_io_request.uart_ops = &k1921vk035_uart_ops_io_request;

    int retval = uart_register(&uart_io_request, &uart_io_request.params) || uart_open(&uart_io_request);
    return retval;
}

STATIC_IRQ_ATTACH(UART0_RX_IRQn, uart_io_request_handler, &io_request);
STATIC_IRQ_ATTACH(UART0_E_RT_IRQn, uart_io_request_handler, &io_request);
