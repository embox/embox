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
#include <limits.h>
#include <stdint.h>
#include <string.h>

#include <kernel/irq_lock.h>
#include <kernel/thread/sched.h>
#include <kernel/thread/event.h>
#include <kernel/work.h>
#include <util/math.h>

static void tty_output(struct tty *t, char ch) {
	tcflag_t lflag = t->termios.c_lflag;
	tcflag_t oflag = t->termios.c_oflag;

	if ((lflag & ICANON) && (oflag & ONLCR) && ch == '\n') {
		// IRQ_LOCKED_DO(tty_rx_enqueue(&t->tx_queue, '\r'));
	}

	// IRQ_LOCKED_DO(tty_rx_enqueue(&t->tx_queue, ch));
}

static void tty_echo(struct tty *t, char ch) {
	tcflag_t lflag = t->termios.c_lflag;

	if (!((lflag & ECHO) || ((lflag & ECHONL) && ch == '\n')))
		return;

	if (iscntrl(ch) && ch != '\n' && ch != '\t' && ch != '\b') {
		tty_output(t, '^');
		ch = toascii(ch + 'A');
	}

	tty_output(t, ch);
}

static void tty_echo_erase(struct tty *t) {
	tcflag_t lflag = t->termios.c_lflag;
	cc_t *cc = t->termios.c_cc;

	if (!(lflag & ECHO))
		return;

	if (lflag & ECHOE)
		tty_output(t, cc[VERASE]);

	else
		for (char *ch = "\b \b"; *ch; ++ch)
			tty_output(t, *ch);
}

/*
 * Input is laid out as follows:
 *
 *                     raw-raw-raw-raw-cooked|cooked|cooked|cooked|editing
 *
 * TTY struct manages two internal rings:
 *       i_ring  [-----***************************************************---]
 * i_canon_ring  [---------------------***************************-----------]
 *
 * Two syntetic rings are also constructed from ends of the two rings above:
 *     raw_ring  [-----****************--------------------------------------]
 *    edit_ring  [------------------------------------------------********---]
 */

static struct ring *tty_raw_ring(struct tty *t, struct ring *r) {
	r->tail = t->i_ring.tail;
	r->head = t->i_canon_ring.tail;
	return r;
}

static struct ring *tty_edit_ring(struct tty *t, struct ring *r) {
	r->tail = t->i_canon_ring.head;
	r->head = t->i_ring.head;
	return r;
}

/* Called in worker context. */
static int tty_input(struct tty *t, char ch, unsigned char flag) {
	tcflag_t iflag = t->termios.c_iflag;
	tcflag_t lflag = t->termios.c_lflag;
	cc_t *cc = t->termios.c_cc;
	int got_data;

	/* Newline control: IGNCR, ICRNL, INLCR */
	if ((iflag & IGNCR) && ch == '\r')
		goto done;
	else if ((iflag & ICRNL) && ch == '\r') ch = '\n';
	else if ((iflag & INLCR) && ch == '\n') ch = '\r';

	/* Handle erase/kill */
	if (lflag & ICANON) {
		int erase_all = (ch == cc[VKILL]);
		if (erase_all || ch == cc[VERASE] || ch == '\b') {

			struct ring edit_ring;
			size_t erase_len = ring_data_size(
						tty_edit_ring(t, &edit_ring), TTY_IO_BUFF_SZ);

			if (erase_len && !erase_all)
				erase_len = 1;

			t->i_ring.head -= erase_len - TTY_IO_BUFF_SZ;
			ring_fixup_head(&t->i_ring, TTY_IO_BUFF_SZ);

			while (erase_len--)
				tty_echo_erase(t);

			goto done;
		}
	}

	/* Finally, store the char. */
	t->i_buff[t->i_ring.head] = ch;

	if (!ring_write(&t->i_ring, TTY_IO_BUFF_SZ, 1))
		assert(0, "Caller must check for possible i_ring overflow");

	tty_echo(t, ch);

done:
	got_data = !(lflag & ICANON) ||
				ch == '\n' || ch == cc[VEOL] || ch == cc[VEOF];

	if (got_data) {
		t->i_canon_ring.head = t->i_ring.head;

		if (!(lflag & ICANON))
			t->i_canon_ring.tail = t->i_ring.tail;
	}

	return got_data;
}

static void tty_rx_worker(struct work *w) {
	struct tty *t = member_cast_out(w, struct tty, rx_work);

	/* no lock if workers are serialized. TODO is it true? -- Eldar */
	while (!ring_full(&t->i_ring, TTY_IO_BUFF_SZ)) {
		int ich = IRQ_LOCKED_DO(tty_rx_dequeue(t));
		if (ich == -1)
			break;

		if (tty_input(t, (char) ich, (unsigned char) (ich>>CHAR_BIT)))
			event_notify(&t->i_event);
	}
}

void tty_post_rx(struct tty *t) {
	work_post(&t->rx_work);
}

/* Must only be called with rx_work disabled */
static char *tty_read_raw(struct tty *t, char *buff, char *end) {
	struct ring raw_ring;
	size_t block_size;

	while ((block_size = ring_can_read(
				tty_raw_ring(t, &raw_ring), TTY_IO_BUFF_SZ, end - buff))) {

		work_enable(&t->rx_work);

		/* No processing is required to read raw data. */
		memcpy(buff, t->i_buff + raw_ring.tail, block_size);
		buff += block_size;

		work_disable(&t->rx_work);

		ring_just_read(&t->i_ring, TTY_IO_BUFF_SZ, block_size);
	}

	return buff;
}

static size_t find_line_len(cc_t eol, cc_t eof,
		const char *buff, size_t size, int *is_eof) {
	size_t offset = 0;

	*is_eof = 0;
	for (; offset < size; ++offset) {
		char ch = buff[offset];
		if (ch == '\n' || ch == eol || ch == eof) {
			*is_eof = (ch == eof);
			break;
		}
	}

	return offset;
}

/* Must only be called with rx_work disabled */
static char *tty_read_cooked(struct tty *t, char *buff, char *end) {
	cc_t eol, eof;
	size_t block_size;

	assert(t->termios.c_lflag & ICANON);

	eol = t->termios.c_cc[VEOL];
	eof = t->termios.c_cc[VEOF];

	while ((block_size = ring_can_read(
				&t->i_canon_ring, TTY_IO_BUFF_SZ, end - buff))) {
		char *line_start = t->i_buff + t->i_canon_ring.tail;
		size_t line_len;
		int got_line;
		int is_eof;

		work_enable(&t->rx_work);

		line_len = find_line_len(eol, eof, line_start, block_size, &is_eof);

		got_line = (line_len < block_size);
		block_size = line_len;

		if (got_line) {
 			++block_size;  /* Line end char is always consumed... */

			if (!is_eof)  /* ...but EOF is discarded and not copied to user. */
	 			++line_len;
		}

		memcpy(buff, line_start, line_len);
		buff += line_len;

		work_disable(&t->rx_work);

		ring_just_read(&t->i_ring, TTY_IO_BUFF_SZ, block_size);
		ring_just_read(&t->i_canon_ring, TTY_IO_BUFF_SZ, block_size);

		if (got_line)
			break;
	}

	return buff;
}

static int tty_wait_input(struct tty *t) {
	int rc;

	sched_lock();

	while (WORK_DISABLED_DO(&t->rx_work, ring_empty(&t->i_ring))) {

		rc = event_wait_ms(&t->i_event, SCHED_TIMEOUT_INFINITE);
		if (rc)
			break;
	}

	sched_unlock();

	return rc;
}

size_t tty_read(struct tty *t, char *buff, size_t size) {
	char *end = buff + size;
	int rc;

	/* TODO what if size is 0? -- Eldar */

	rc = tty_wait_input(t);
	if (rc == -EINTR)
		/* TODO then what? -- Eldar */
		return 0;

	work_disable(&t->rx_work);
	{
		buff = tty_read_raw(t, buff, end);

		/* TODO serialize termios access with ioctl. -- Eldar */
		if (t->termios.c_lflag & ICANON) {
			buff = tty_read_cooked(t, buff, end);
		}
	}
	work_enable(&t->rx_work);

	return buff - (end - size);
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

	work_init(&t->rx_work, tty_rx_worker, 0);
	ring_init(&t->rx_ring);

	event_init(&t->i_event, "tty input");
	ring_init(&t->i_ring);
	ring_init(&t->i_canon_ring);

	event_init(&t->o_event, "tty output");
	ring_init(&t->o_ring);

	return t;
}


int tty_rx_enqueue(struct tty *t, char ch, unsigned char flag) {
	uint16_t *slot = t->rx_buff + t->rx_ring.head;

	if (!ring_write(&t->rx_ring, TTY_RX_BUFF_SZ, 1))
		return -1;

	*slot = (flag<<CHAR_BIT) | (unsigned char) ch;
	return 0;
}

int tty_rx_dequeue(struct tty *t) {
	uint16_t *slot = t->rx_buff + t->rx_ring.tail;

	if (!ring_read(&t->rx_ring, TTY_RX_BUFF_SZ, 1))
		return -1;

	return (int) *slot;
}


