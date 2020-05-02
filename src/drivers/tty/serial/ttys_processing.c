/**
 * @file
 *
 * @date 25.04.2016
 * @author Anton Bondarev
 */

#include <mem/misc/pool.h>
#include <util/dlist.h>

#include <drivers/tty.h>
#include <drivers/ttys.h>
#include <drivers/serial/uart_device.h>
#include <drivers/irqctrl.h>
#include <kernel/lthread/lthread.h>

#include <embox/unit.h>

EMBOX_UNIT_INIT(idesc_serial_init);


#define UART_DATA_BUFF_SZ 8
#define UART_RX_HND_PRIORITY 128


static struct lthread uart_rx_irq_handler;

struct uart_rx {
	struct uart *uart;
	int data;
	struct dlist_head lnk;
};

POOL_DEF(uart_rx_buff, struct uart_rx, UART_DATA_BUFF_SZ);

static DLIST_DEFINE(uart_rx_list);
static int uart_rx_action(struct lthread *self);

static inline struct uart *tty2uart(struct tty *tty) {
	struct tty_uart *tu;
	tu = member_cast_out(tty, struct tty_uart, tty);
	return tu->uart;
}

static int uart_rx_buff_put(unsigned int irq_nr, struct uart *dev) {
	struct uart_rx *rx;

	if (dev->tty) {
		while (uart_hasrx(dev)) {
			irq_lock();
			{
				rx = pool_alloc(&uart_rx_buff);
				if (!rx) {
					irqctrl_disable(dev->irq_num);
					lthread_launch(&uart_rx_irq_handler);
					
					irqctrl_enable(dev->irq_num);
					rx = pool_alloc(&uart_rx_buff);
					if (!rx) {
						irq_unlock();
						return -1;
					}
				}

				rx->uart = dev;
				rx->data = uart_getc(dev);
				dlist_add_prev(dlist_head_init(&rx->lnk), &uart_rx_list);
			}
			irq_unlock();
		}
		lthread_launch(&uart_rx_irq_handler);
	}
	
	return 0;
}

static int uart_rx_buff_get(struct uart_rx *rx_data) {
	struct uart_rx *rx;

	if (dlist_empty(&uart_rx_list)) {
		return -1;
	}

	irq_lock();
	{
		rx = dlist_next_entry_or_null(&uart_rx_list, struct uart_rx, lnk);
		if (!rx) {
			irq_unlock();
			return -1;
		}

		*rx_data = *rx;
		dlist_del(&rx->lnk);
		pool_free(&uart_rx_buff, rx);
	}
	irq_unlock();

	return 0;
}

static int uart_rx_action(struct lthread *self) {
	struct uart_rx rx;

	while (!uart_rx_buff_get(&rx)) {
		tty_rx_locked(rx.uart->tty, rx.data, 0);
	}

	return 0;
}

irq_return_t uart_irq_handler(unsigned int irq_nr, void *data) {
	struct uart *dev = data;

	uart_rx_buff_put(irq_nr, dev);

	return IRQ_HANDLED;
}

static int idesc_serial_init(void) {
	lthread_init(&uart_rx_irq_handler, &uart_rx_action);
	schedee_priority_set(&uart_rx_irq_handler.schedee, UART_RX_HND_PRIORITY);
	return 0;
}
