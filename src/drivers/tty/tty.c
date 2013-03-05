/**
 * @file
 * @brief
 *
 * @date 05.03.13
 * @author Eldar Abusalimov
 */

#include <drivers/tty.h>

#include <stdint.h>
#include <ctype.h>

#include <kernel/irq_lock.h>

struct tty_queue *tty_queue_init(struct tty_queue *tq) {
	tq->offset = tq->count = 0;
	return tq;
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

