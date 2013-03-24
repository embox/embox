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

#define TC_I(t, flag) ((t)->termios.c_iflag & (flag))
#define TC_O(t, flag) ((t)->termios.c_oflag & (flag))
#define TC_C(t, flag) ((t)->termios.c_cflag & (flag))
#define TC_L(t, flag) ((t)->termios.c_lflag & (flag))

static void tty_tx_char(struct tty *t, char ch) {
	// TODO locks? context? -- Eldar
	ring_write_all_from(&t->o_ring, t->o_buff, TTY_IO_BUFF_SZ, &ch, 1);
	// t->ops->tx_char(t, ch);
}

/* Called in worker-protected context. */
static void tty_output(struct tty *t, char ch) {
	if (TC_L(t, ICANON) && TC_O(t, ONLCR) && ch == '\n')
		tty_tx_char(t, '\r');

	tty_tx_char(t, ch);
}

static void tty_echo(struct tty *t, char ch) {
	if (!(TC_L(t, ECHO) || (TC_L(t, ECHONL) && ch == '\n')))
		return;

	if (iscntrl(ch) && ch != '\n' && ch != '\t' && ch != '\b') {
		tty_output(t, '^');
		if (ch != 0377)
			ch += 'A' - 1;  /* ('A' - 1) == ('@') == ('\0' + 0x40). */
		else
			ch = '?';  /* ASCII DEL */
	}

	tty_output(t, ch);
}

static void tty_echo_erase(struct tty *t) {
	cc_t *cc = t->termios.c_cc;

	if (!TC_L(t, ECHO))
		return;

	if (TC_L(t, ECHOE))
		tty_output(t, cc[VERASE]);

	else
		for (char *ch = "\b \b"; *ch; ++ch)
			tty_output(t, *ch);
}

/*
 * Input layout is the following:
 *
 *       i_buff  [     raw-raw-raw-raw-cooked|cooked|cooked|cooked|editing   ]
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
	cc_t *cc = t->termios.c_cc;
	int ignore_cr;
	int raw_mode;
	int is_eol;
	int got_data;

	raw_mode = !TC_L(t, ICANON);

	/* Newline control: IGNCR, ICRNL, INLCR */
	ignore_cr = TC_I(t, IGNCR) && ch == '\r';
	if (!ignore_cr) {
		if (TC_I(t, ICRNL) && ch == '\r') ch = '\n';
		if (TC_I(t, INLCR) && ch == '\n') ch = '\r';
	}
	is_eol = (ch == '\n' || ch == cc[VEOL]);

	if (ignore_cr)
		goto done;

	/* Handle erase/kill */
	if (!raw_mode) {
		int erase_all = (ch == cc[VKILL]);
		if (erase_all || ch == cc[VERASE] || ch == '\b') {

			struct ring edit_ring;
			size_t erase_len = ring_data_size(
						tty_edit_ring(t, &edit_ring), TTY_IO_BUFF_SZ);

			if (erase_len) {
				if (!erase_all)
					erase_len = 1;

				t->i_ring.head -= erase_len - TTY_IO_BUFF_SZ;
				ring_fixup_head(&t->i_ring, TTY_IO_BUFF_SZ);

				while (erase_len--)
					tty_echo_erase(t);
			}

			goto done;
		}
	}

	/* Finally, store and echo the char.
	 *
	 * When i_ring is near to become full, only raw or a line ending chars are
	 * handled. This lets canonical read to see the line with \n or EOL at the
	 * end, even when some chars are missing. */

	if (ring_room_size(&t->i_ring, TTY_IO_BUFF_SZ) > !(raw_mode || is_eol))
		if (ring_write_all_from(&t->i_ring, t->i_buff, TTY_IO_BUFF_SZ, &ch, 1))
			tty_echo(t, ch);

done:
	got_data = (raw_mode || is_eol || ch == cc[VEOF]);

	if (got_data) {
		t->i_canon_ring.head = t->i_ring.head;

		if (raw_mode)
			/* maintain it empty */
			t->i_canon_ring.tail = t->i_canon_ring.head;
	}

	return got_data;
}

static void tty_rx_worker(struct work *w) {
	struct tty *t = member_cast_out(w, struct tty, rx_work);
	int ich;
	char ch;

	/* no worker locks if workers are serialized. TODO is it true? -- Eldar */

	irq_lock();
	while ((ich = tty_rx_dequeue(t)) != -1) {
		irq_unlock();

		if (tty_input(t, (char) ich, (unsigned char) (ich>>CHAR_BIT)))
			event_notify(&t->i_event);

		irq_lock();
	}
	work_pending_reset(w);
	irq_unlock();

	while (ring_read_all_into(&t->o_ring, t->o_buff, TTY_IO_BUFF_SZ, &ch, 1)) {
		t->ops->tx_char(t, ch);
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

	assert(TC_L(t, ICANON));

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
		if (TC_L(t, ICANON)) {
			buff = tty_read_cooked(t, buff, end);
		}
	}
	work_enable(&t->rx_work);

	return buff - (end - size);
}

size_t tty_write(struct tty *t, const char *buff, size_t size) {
	size_t count;

	work_disable(&t->rx_work);

	count = ring_write_all_from(&t->o_ring, t->o_buff, TTY_IO_BUFF_SZ,
			 buff, size);
	work_post(&t->rx_work);

	work_enable(&t->rx_work);

	return count;
}

int tty_ioctl(struct tty *tty, int request, void *data) {
	switch (request) {
	case TIOCGETA:
		memcpy(&tty->termios, data, sizeof(struct termios));
		break;
	case TIOCSETAF:
	case TIOCSETAW:
	case TIOCSETA:
		memcpy(data, &tty->termios, sizeof(struct termios));
		break;
	default:
		return -ENOSYS;
	}

	return ENOERR;
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


