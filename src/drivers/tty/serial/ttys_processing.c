/**
 * @file
 *
 * @date 25.04.2016
 * @author Anton Bondarev
 */
#include <drivers/serial/uart_dev.h>
#include <drivers/tty.h>
#include <drivers/ttys.h>
#include <embox/unit.h>
#include <kernel/lthread/lthread.h>
#include <mem/misc/pool.h>
#include <lib/libds/dlist.h>
#include <util/log.h>

EMBOX_UNIT_INIT(idesc_serial_init);

#define UART_RX_HND_PRIORITY 128

static struct lthread uart_rx_irq_handler;

static int uart_rx_buff_put(struct uart *dev) {
	int ch;

	if (dev->tty) {
		while (uart_hasrx(dev)) {
			irq_lock();
			{
				uart_state_set(dev, UART_STATE_RX_ACTIVE);

				if (ring_full(&dev->uart_rx_ring.ring,
				        dev->uart_rx_ring.capacity)) {
					if (dev->uart_ops->uart_irq_dis) {
						dev->uart_ops->uart_irq_dis(dev, &dev->params);
					}
					lthread_launch(&uart_rx_irq_handler);
					irq_unlock();
					return -1;
				}

				ch = uart_getc(dev);
				if (1 != ring_buff_enqueue(&dev->uart_rx_ring, &ch, 1)) {
					assert(0);
				}
			}
			irq_unlock();
		}
		lthread_launch(&uart_rx_irq_handler);
	}

	return 0;
}

static int uart_rx_action(struct lthread *self) {
	struct uart *uart = NULL;

	uart_opened_foreach(uart) {
		int ch;

		if (uart_state_test(uart, UART_STATE_RX_ACTIVE)) {
			uart_state_clear(uart, UART_STATE_RX_ACTIVE);
			while (ring_buff_dequeue(&uart->uart_rx_ring, &ch, 1)) {
				tty_rx_locked(uart->tty, ch, 0);
			}
			if (uart->uart_ops->uart_irq_en) {
				uart->uart_ops->uart_irq_en(uart, &uart->params);
			}
		}
	}

	return 0;
}

irq_return_t uart_irq_handler(unsigned int irq_nr, void *data) {
	struct uart *dev = data;

	log_debug("irq_nr %d", irq_nr);
	uart_rx_buff_put(dev);

	return IRQ_HANDLED;
}

static int idesc_serial_init(void) {
	lthread_init(&uart_rx_irq_handler, &uart_rx_action);
	schedee_priority_set(&uart_rx_irq_handler.schedee, UART_RX_HND_PRIORITY);
	return 0;
}
