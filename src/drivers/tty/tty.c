/**
 * @file
 * @brief
 *
 * @date 05.03.13
 * @author Eldar Abusalimov
 */

#include <drivers/tty.h>

#include <ctype.h>
#include <errno.h>
#include <stdint.h>
#include <string.h>

#include <kernel/irq_lock.h>
#include <kernel/thread/sched.h>
#include <kernel/thread/event.h>
#include <kernel/work.h>

static void tty_output(struct tty *t, char ch) {
	tcflag_t lflag = t->termios.c_lflag;
	tcflag_t oflag = t->termios.c_oflag;

	if ((lflag & ICANON) && (oflag & ONLCR) && ch == '\n') {
		// IRQ_LOCKED_DO(tty_enqueue(&t->tx_queue, '\r'));
	}

	// IRQ_LOCKED_DO(tty_enqueue(&t->tx_queue, ch));
}

static void tty_echo(struct tty *t, char ch) {
	tcflag_t lflag = t->termios.c_lflag;

	if (!((lflag & ECHO) || ((lflag & ECHONL) && ch == '\n'))) {
		return;
	}

	if (iscntrl(ch) && ch != '\n' && ch != '\t' && ch != '\b') {
		tty_output(t, '^');
		ch = toascii(ch + 'A');
	}

	tty_output(t, ch);
}

static void tty_echo_erase(struct tty *t) {
	tcflag_t lflag = t->termios.c_lflag;
	cc_t *cc = t->termios.c_cc;

	if (!(lflag & ECHO)) {
		return;
	}

	if (lflag & ECHOE) {
		tty_output(t, cc[VERASE]);

	} else {
		for (char *ch = "\b \b"; *ch; ++ch) {
			tty_output(t, *ch);
		}
	}
}

static int tty_input(struct tty *t, char ch) {
	tcflag_t iflag = t->termios.c_iflag;
	tcflag_t lflag = t->termios.c_lflag;
	cc_t *cc = t->termios.c_cc;

	/* Newline control: IGNCR, ICRNL, INLCR */
	if ((iflag & IGNCR) && ch == '\r') {
		goto done;
	} else if ((iflag & ICRNL) && ch == '\r') {
		ch = '\n';
	} else if ((iflag & INLCR) && ch == '\n') {
		ch = '\r';
	}

	/* Handle erase/kill */
	if (lflag & ICANON) {
		int erase_all = (ch == cc[VKILL]);
		if (erase_all || ch == cc[VERASE] || ch == '\b') {
			while (tty_dequeue(&t->local_queue) != -1) {
				tty_echo_erase(t);
				if (!erase_all) {
					break;
				}
			}
			goto done;
		}
	}

	if (tty_enqueue(&t->local_queue, ch) == -1) {
		assert(0, "Caller must check for possible queue overflow beforehand");
	}

	tty_echo(t, ch);

done:
	return !(lflag & ICANON) || ch == '\n' || ch == cc[VEOL] || ch == cc[VEOF];
}

static void tty_rx_worker(struct work *w) {
	struct tty *t = member_cast_out(w, struct tty, rx_work);

	while (!tty_queue_full(&t->local_queue)) {
		int ich = IRQ_LOCKED_DO(tty_dequeue(&t->rx_queue));
		if (ich == -1) {
			break;
		}

		if (tty_input(t, ich)) {
			event_notify(&t->read_event);
		}
	}
}

void tty_post_rx(struct tty *t) {
	work_post(&t->rx_work);
}

size_t tty_read(struct tty *t, char *buff, size_t size) {
	tcflag_t lflag = t->termios.c_lflag;
	cc_t *cc = t->termios.c_cc;
	size_t count = 0;
	int rc;

	sched_lock();

	while (WORK_DISABLED_DO(&t->rx_work, tty_queue_empty(&t->local_queue))) {
		rc = event_wait_ms(&t->read_event, SCHED_TIMEOUT_INFINITE);
		if (rc == -EINTR) {
			/* TODO then what? -- Eldar */
			sched_unlock();
			return 0;
		}
	}

	sched_unlock();

	while (count < size) {
		int ich = tty_dequeue(&t->local_queue);
		char ch = ich;
		if (ich == -1) {
			break;
		}

		if ((lflag & ICANON) && ch == cc[VEOF]) {
			break;
		}

		*buff = ch;
		++count;

		if ((lflag & ICANON) && (ch == '\n' || ch == cc[VEOL])) {
			break;
		}
		++buff;
	}

	return count;
}

struct tty *tty_init(struct tty *t, struct tty_ops *ops) {
	assert(t && ops);

	t->ops = ops;

	{
		struct termios *termios = &t->termios;
		cc_t cc_init[NCCS] = TTY_TERMIOS_CC_INIT;

		memcpy(termios->c_cc, cc_init, sizeof(cc_init));
		termios->c_iflag = TTY_TERMIOS_IFLAG_INIT;
		termios->c_oflag = TTY_TERMIOS_OFLAG_INIT;
		termios->c_cflag = TTY_TERMIOS_CFLAG_INIT;
		termios->c_lflag = TTY_TERMIOS_LFLAG_INIT;
	}

	tty_queue_init(&t->rx_queue);
	work_init(&t->rx_work, tty_rx_worker);

	event_init(&t->read_event, "tty read");

	return t;
}

int tty_enqueue(struct tty_queue *tq, char ch) {
	size_t tail_offset;

	if (tty_queue_full(tq)) {
		return -1;
	}

	tail_offset = tq->offset + tq->count;

	if (tail_offset >= TTY_QUEUE_SZ) {
		tail_offset -= TTY_QUEUE_SZ;
	}
	tq->buff[tail_offset] = ch;

	++tq->count;

	return 0;
}

int tty_dequeue(struct tty_queue *tq) {
	char ch;

	if (tty_queue_empty(tq)) {
		return -1;
	}

	ch = tq->buff[tq->offset];

	++tq->offset;
	if (tq->offset == TTY_QUEUE_SZ) {
		tq->offset = 0;
	}

	--tq->count;

	return (int) ch;
}

struct tty_queue *tty_queue_init(struct tty_queue *tq) {
	tq->offset = tq->count = 0;
	return tq;
}

