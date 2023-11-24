#include <drivers/serial/uart_dev.h>
#include <drivers/serial/diag_serial.h>
#include <drivers/gpio/gpio.h>
#include <drivers/ttys.h>
#include "K1921VK035.h"
#include "errno.h"
#include "kernel/irq_lock.h"
#include "plib035_uart.h"
#include "string.h"
#include "time.h"
#include "util/ring_buff.h"
#include <embox/unit.h>
#include <kernel/time/ktime.h>
#include <sys/uio.h>
#include <drivers/serial/k1921vk035_io_request.h>

extern const struct uart_ops k1921vk035_uart_ops;
#include <kernel/printk.h>

void io_request_clean(io_request_t* io) {
    io->type = IO_REQUEST_MODE_NONE;
    memset(&io->mode, 0, sizeof(io->mode));
}

irq_return_t uart_io_request_handler(unsigned int irq_nr, void *data) {
    io_request_t* io_req = (io_request_t* )data;
    // printk("I %d, RIS: 0x%x, HRX %d\n", irq_nr, ((UART_TypeDef* )io_req->uart->base_addr)->RIS, uart_hasrx(io_req->uart));
    irq_lock();
    while(uart_hasrx(io_req->uart)) {
        int ch = uart_getc(io_req->uart);
        // printk("HRX %d %2.X\n", uart_hasrx(io_req->uart), ch);
        switch(io_req->type) {
            case IO_REQUEST_MODE_NONE:
            {
                /* Store in intermediate buffer */
                int res = ring_buff_enqueue(io_req->rx_buff, &ch, 1);
                if(res == 0) {
                    /* TODO: no one wants to read out data so intermediate buffer is overflowed, what do we do? */
                }
            } break;

            case IO_REQUEST_MODE_READ:
            {
                volatile io_mode_read* read = &io_req->mode.read;
                read->buffer[0] = ch;
                read->buffer++;
                read->count--;
                if(read->count == 0) {
                    io_request_clean(io_req);
                    /* Notify thread that was waiting */
                    sem_post(&io_req->semaphore);
                }
            } break;

            case IO_REQUEST_MODE_DISCARD:
            {
                /* Do nothing with byte */
                volatile io_mode_discard* discard = &io_req->mode.discard;
                discard->count--;
                if(discard->count == 0) {
                    io_request_clean(io_req);
                }
            } break;

        }
    }
    irq_unlock();
    return IRQ_HANDLED;
}


int io_request_read(io_request_t* io, char* buf, size_t count) {
    irq_lock();
    {
        /* use buffer first */
        int ring_count = ring_buff_get_cnt(io->rx_buff);
        if(ring_count >= count) {
            /* we can satisfy call right now */
            for(int i = 0; i < count; i++) {
                ring_buff_dequeue(io->rx_buff, buf + i, 1);
            }
            irq_unlock();
            return count;
        } else {
            for(int i = 0; i < ring_count; i++) {
                ring_buff_dequeue(io->rx_buff, buf + i, 1);
            }
            io->type = IO_REQUEST_MODE_READ;
            io->mode.read.buffer = buf + ring_count;
            io->mode.read.count = count - ring_count;
        }
    }

    irq_unlock();
    sem_wait(&io->semaphore);
    return count;
}

int io_request_readv(io_request_t* io, struct iovec* iov, size_t iov_count) {
    int ret = 0;
    for(int i = 0; i < iov_count; i++) {
        ret += io_request_read(io, iov[i].iov_base, iov[i].iov_len);
    }
    return ret;
}

int io_request_read_timeout(io_request_t* io, char* buf, size_t count, int32_t timeout_ms) {
    irq_lock();
    int ring_count = ring_buff_get_cnt(io->rx_buff);
    int read_count = ring_count > count ? count : ring_count;
    if(timeout_ms < 0) {
        /* No timeout, flush all we got */
        for(int i = 0; i < read_count; i++) {
            ring_buff_dequeue(io->rx_buff, buf + i, 1);
        }
        irq_unlock();
        return read_count;
    } else {
        if(ring_count >= count) {
            /* we can satisfy call right now */
            for(int i = 0; i < count; i++) {
                ring_buff_dequeue(io->rx_buff, buf + i, 1);
            }
            irq_unlock();
            return count;
        } else {
            for(int i = 0; i < ring_count; i++) {
                ring_buff_dequeue(io->rx_buff, buf + i, 1);
            }
            io->type = IO_REQUEST_MODE_READ;
            io->mode.read.buffer = buf + ring_count;
            io->mode.read.count = count - ring_count;
        }
    }

    irq_unlock();
    // sched_wait_timeout(clock_t timeout, clock_t *remain);
    struct timespec current_time = {};
    clock_gettime(CLOCK_REALTIME, &current_time);
    struct timespec wait_time = timespec_add(current_time, ns_to_timespec((uint64_t)(timeout_ms) * 1000000));\
    int wait_res = sem_timedwait(&io->semaphore, &wait_time);
    if(wait_res == -ETIMEDOUT) {
        int ret = count - io->mode.read.count;
        io_request_clean(io);
        return ret;
    }
    return count;
}

int io_request_readv_timeout(io_request_t* io, struct iovec* iov, size_t iov_count, int32_t timeout_ms_per_iov) {
    int ret = 0;
    for(int i = 0; i < iov_count; i++) {
        int iov_ret = io_request_read_timeout(io, iov[i].iov_base, iov[i].iov_len, timeout_ms_per_iov);
        ret += iov_ret;
        if(iov_ret < iov[i].iov_len) {
            /* timeout */
            break;
        }
    }

    return ret;
}

int io_request_write(io_request_t* io, char* buf, size_t count) {
    for(size_t i = 0; i < count; i++) {
        uart_putc(io->uart, buf[i]);
    }
    return count;
}

/* NOTE TODO: this should only be invoked while driver is in IO_REQUEST_MODE_NONE mode - for
 * now we dont have any checks because if drivers goes to IO_REQUEST_MODE_READ mode driver does
 * not return to user until it goes to IO_REQUEST_MODE_NONE mode again */
void io_request_discard_next_bytes(io_request_t* io, size_t count) {
    if(count > 0) {
        io->type = IO_REQUEST_MODE_DISCARD;
        io->mode.discard.count = count;
    }
}
