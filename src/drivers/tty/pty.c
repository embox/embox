/**
 * @file
 * @brief Pseudo TTY master-side and slave driver.
 *
 * @date 25.03.13
 * @author Eldar Abusalimov
 */
#include <poll.h>
#include <drivers/pty.h>
#include <drivers/tty.h>

#include <string.h>
#include <util/member.h>

#include <kernel/sched.h>
//#include <kernel/event.h>
#include <fs/idesc.h>
#include <fs/idesc_event.h>
#include <kernel/work.h>

static void pty_out_wake(struct tty *t) {
	if (t->idesc) {
		idesc_notify(t->idesc, POLLIN);
	}
}

static inline int pty_tty_read(struct tty *t, char *buff, size_t size) {
	size_t block_size;

	while ((block_size = ring_can_read(&t->o_ring, TTY_IO_BUFF_SZ, size))) {
		const char *block = t->o_buff + t->o_ring.tail;

		work_enable(&t->rx_work);

		memcpy(buff, block, block_size);
		buff += block_size;
		size -= block_size;

		work_disable(&t->rx_work);

		ring_just_read(&t->o_ring, TTY_IO_BUFF_SZ, block_size);
		return block_size;
	}

	return block_size;
}

static inline int pty_wait(struct idesc *idesc, struct tty *t, int flags) {
	struct idesc_wait_link wl;
	int res;

	res = idesc_wait_prepare(idesc, &wl, flags);
	if (!res) {
		work_enable(&t->rx_work);

		res = idesc_wait(idesc, flags, SCHED_TIMEOUT_INFINITE);

		work_disable(&t->rx_work);
		idesc_wait_cleanup(idesc, &wl);
	}
	return res;
}

size_t pty_read(struct pty *pt, struct idesc *idesc, char *buff, size_t size) {
	struct tty *t = pty_to_tty(pt);
	size_t block_size;
	int ret;

	work_disable(&t->rx_work);
	{
		do {
			block_size = pty_tty_read(t, buff, size);
			if (block_size) {
				ret = block_size;
				break;
			}

			//t->idesc = idesc;
			ret = pty_wait(idesc, t, POLLIN | POLLERR);
			//t->idesc = NULL;
		} while (!ret);
	}
	work_enable(&t->rx_work);

	return ret;
}

size_t pty_write(struct pty *pt, const char *buff, size_t size) {
	struct tty *t = pty_to_tty(pt);
	const char *saved_buff = buff;

	while (size--)
		tty_rx_putc(t, *buff++, 0);

	return buff - saved_buff;
}

void pty_notify(struct pty *pt) {
	struct tty *t = pty_to_tty(pt);

	pty_out_wake(t);

}

const struct tty_ops pty_ops = {
	.out_wake = pty_out_wake,
};

struct pty *pty_init(struct pty *p, struct idesc *idesc) {

	p->tty.idesc = idesc;
	tty_init(pty_to_tty(p), &pty_ops);

	return p;
}

