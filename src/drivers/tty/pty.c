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
#include <fs/idesc.h>
#include <fs/idesc_event.h>

static void pty_out_wake(struct tty *t) {
	struct pty *pty = pty_from_tty(t);

	idesc_notify(pty->master, POLLIN);
}

static inline int pty_wait(struct idesc *idesc, struct tty *t, int flags) {
	struct idesc_wait_link wl;

	return IDESC_WAIT_LOCKED(mutex_unlock(&t->lock),
			idesc, &wl, flags, SCHED_TIMEOUT_INFINITE,
			mutex_lock(&t->lock));
}

size_t pty_read(struct pty *pt, struct idesc *idesc, char *buff, size_t size) {
	struct tty *t = pty_to_tty(pt);
	size_t block_size;
	int ret;

	mutex_lock(&t->lock);
	{
		do {
			block_size = tty_out_buf(t, buff, size);
			if (block_size) {
				ret = block_size;
				break;
			}

			ret = pty_wait(idesc, t, POLLIN | POLLERR);
		} while (!ret);
	}
	mutex_unlock(&t->lock);

	return ret;
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

struct pty *pty_init(struct pty *p, struct idesc *master, struct idesc *slave) {

	tty_init(pty_to_tty(p), &pty_ops);
	p->tty.idesc = slave;
	p->master = master;

	return p;
}
