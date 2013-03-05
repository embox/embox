/**
 * @file
 * @brief
 *
 * @date 05.03.13
 * @author Eldar Abusalimov
 */

#include <drivers/tty.h>

#include <stdint.h>
#include <string.h>
#include <ctype.h>

#include <kernel/irq_lock.h>

/* Called from inside IRQ, beware. */
void tty_rx_char(struct tty *t, char ch) {
	// NIY
}

struct tty *tty_init(struct tty *t, struct tty_ops *ops) {
	assert(t && ops);

	t->ops = ops;

	{
		struct termios *termios = &t->termios;
		cc_t cc_init[NCCS] = TTY_TERMIOS_CC_INIT;

		memcpy(&termios->c_cc, cc_init, sizeof(cc_init));
		termios->c_iflag = TTY_TERMIOS_IFLAG_INIT;
		termios->c_oflag = TTY_TERMIOS_OFLAG_INIT;
		termios->c_cflag = TTY_TERMIOS_CFLAG_INIT;
		termios->c_lflag = TTY_TERMIOS_LFLAG_INIT;
	}

	tty_queue_init(&t->rx_queue);
	tty_queue_init(&t->tx_queue);

	event_init(&t->rx_event, "tty rx");
	event_init(&t->tx_event, "tty tx");

	return t;
}

int tty_enqueue(struct tty_queue *tq, char ch) {
	int is_full;

	irq_lock();

	is_full = (tq->count == TTY_QUEUE_SZ);
	if (!is_full) {
		size_t tail_off = tq->offset + tq->count;

		if (tail_off >= TTY_QUEUE_SZ) {
			tail_off -= TTY_QUEUE_SZ;
		}
		tq->buff[tail_off] = ch;

		++tq->count;
	}

	irq_unlock();

	return -is_full;
}

int tty_dequeue(struct tty_queue *tq) {
	int ret;

	irq_lock();

	if (tq->count) {
		ret = (int) tq->buff[tq->offset];

		++tq->offset;
		if (tq->offset == TTY_QUEUE_SZ) {
			tq->offset = 0;
		}

		--tq->count;
	} else {
		ret = -1;
	}

	irq_unlock();

	return ret;
}

struct tty_queue *tty_queue_init(struct tty_queue *tq) {
	tq->offset = tq->count = 0;
	return tq;
}

