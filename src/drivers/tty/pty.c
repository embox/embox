/**
 * @file
 * @brief Pseudo TTY master-side and slave driver.
 *
 * @date 25.03.13
 * @author Eldar Abusalimov
 */

#include <drivers/pty.h>

#include <string.h>
#include <util/member.h>

#include <kernel/sched.h>
#include <kernel/event.h>
#include <kernel/work.h>

static void pty_out_wake(struct tty *t) {
#if 0
	struct pty *pt = pty_from_tty(t);

	event_notify(&pt->read_event);
#endif
}

size_t pty_read(struct pty *pt, char *buff, size_t size) {
	struct tty *t = pty_to_tty(pt);
	const char *saved_buff = buff;
	size_t block_size;
#if 0
	int rc;
#endif

	for (;;) {
		work_disable(&t->rx_work);
		while ((block_size = ring_can_read(&t->o_ring, TTY_IO_BUFF_SZ, size))) {
			const char *block = t->o_buff + t->o_ring.tail;

			work_enable(&t->rx_work);

			memcpy(buff, block, block_size);
			buff += block_size;
			size -= block_size;

			work_disable(&t->rx_work);

			ring_just_read(&t->o_ring, TTY_IO_BUFF_SZ, block_size);
		}
		work_enable(&t->rx_work);

		if (!size)
			break;
#if 0

		rc = event_wait_ms(&pt->read_event, SCHED_TIMEOUT_INFINITE);
		if (rc)
			break;
#endif
	}

	return buff - saved_buff;
}

size_t pty_write(struct pty *pt, const char *buff, size_t size) {
	struct tty *t = pty_to_tty(pt);
	const char *saved_buff = buff;

	while (size--)
		tty_rx_putc(t, *buff++, 0);

	return buff - saved_buff;
}

const struct tty_ops pty_ops = {
	.out_wake = pty_out_wake,
};

struct pty *pty_init(struct pty *p) {
	tty_init(pty_to_tty(p), &pty_ops);

	return p;
}

