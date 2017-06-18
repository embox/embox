/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    08.08.2013
 */

#include <ctype.h>
#include <string.h>
#include <termios.h>
#include <util/ring.h>

#include <drivers/tty/termios_ops.h>
#include <kernel/thread/thread_sched_wait.h>

#define TIO_I(t, flag) ((t)->c_iflag & (flag))
#define TIO_O(t, flag) ((t)->c_oflag & (flag))
#define TIO_C(t, flag) ((t)->c_cflag & (flag))
#define TIO_L(t, flag) ((t)->c_lflag & (flag))

static int tio_putc(char ch, struct ring *ring,
		char *buf, size_t buflen) {
	return ring_write_all_from(ring, buf, buflen, &ch, 1);
}

int termios_putc(const struct termios *t, char ch, struct ring *ring,
		char *buf, size_t buflen) {
	
	int res = 0;

	if (TIO_L(t, ICANON) && TIO_O(t, ONLCR) && ch == '\n') {
		if (2 > ring_room_size(ring, buflen)) {
			return 0;
		}

		res += tio_putc('\r', ring, buf, buflen);
	}

	return res + tio_putc(ch, ring, buf, buflen);
}

int termios_gotc(const struct termios *t, char ch, struct ring *ring,
		char *buf, size_t buflen) {
	
	int res = 0;

	if (TIO_L(t, ECHO) || (TIO_L(t, ECHONL) && ch == '\n')) {
		if (iscntrl(ch) && ch != '\n' && ch != '\t' && ch != '\b') {
			/* ASCII table magic:  CTRL(ch) -> ^ch;  ASCII DEL -> ^? */
			res += termios_putc(t, '^', ring, buf, buflen);
			ch = toascii(ch + 'A' - 1);  /* ('A'-1) == ('@') == ('\0'+0x40). */
		}

		res += termios_putc(t, ch, ring, buf, buflen);
	}

	return res;
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

struct ring *termios_edit_ring(struct ring *r, struct ring *i_ring,
	struct ring *i_canon_ring) {
	
	r->tail = i_canon_ring->head;
	r->head = i_ring->head;
	return r;
}

struct ring *termios_raw_ring(struct ring *r, struct ring *i_ring,
	struct ring *i_canon_ring) {

	r->tail = i_ring->tail;
	r->head = i_canon_ring->tail;
	return r;
}

int termios_echo_status(const struct termios *t, char *verase) {
	if (!TIO_L(t, ECHOE)) {
		*verase = t->c_cc[VERASE];
		return ECHO;
	}
	else {
		return ECHOE;
	}
}

int termios_handle_newline(const struct termios *t, char ch, int *is_eol) {
	int ignore_cr = TIO_I(t, IGNCR) && ch == '\r';

	if (!ignore_cr) {
		if (TIO_I(t, ICRNL) && ch == '\r') {
			ch = '\n';
		}

		if (TIO_I(t, INLCR) && ch == '\n') {
			ch = '\r';
		}
	}
	
	*is_eol = (ch == '\n' || ch == t->c_cc[VEOL]);

	return ignore_cr;
}

int termios_handle_erase(const struct termios *t, char ch, int *raw_mode,
	struct ring *i_ring, struct ring *i_canon_ring, size_t buflen) {

	*raw_mode = !TIO_L(t, ICANON);
	int erase_all = (ch == t->c_cc[VKILL]);

	if (erase_all || ch == t->c_cc[VERASE] || ch == '\b') {

		struct ring edit_ring;
		size_t erase_len = ring_data_size(
				termios_edit_ring(&edit_ring, i_ring, i_canon_ring), buflen);

		if (erase_len) {
			if (!erase_all) {
				erase_len = 1;
			}

			i_ring->head -= erase_len - buflen;
			ring_fixup_head(i_ring, buflen);
		}

		return erase_len;
	}

	return -1;
}

int termios_input_status(const struct termios *t, char ch,
	struct ring *i_ring, struct ring *i_canon_ring) {
	
	int raw_mode = !TIO_L(t, ICANON);
	int is_eol = (ch == '\n' || ch == t->c_cc[VEOL]);
	int got_data = (raw_mode || is_eol || ch == t->c_cc[VEOF]);

	if (got_data) {
		i_canon_ring->head = i_ring->head;

		if (raw_mode) {
			/* maintain it empty */
			i_canon_ring->tail = i_canon_ring->head;
		}
	}

	return got_data;
}

size_t termios_find_line_len(const struct termios *t, 
	const char *buff, size_t size, int *is_eof) {
	
	size_t offset;

	*is_eof = 0;
	
	for (offset = 0; offset < size; ++offset) {
		char ch = buff[offset];

		if (ch == '\n' || ch == t->c_cc[VEOL] || ch == t->c_cc[VEOF]) {
			*is_eof = (ch == t->c_cc[VEOF]);
			break;
		}
	}

	return offset;
}

char *termios_read_cooked(const struct termios *t, char *buff, char *end,
	struct ring *i_ring, struct ring *i_canon_ring, char *i_buff, size_t buflen) {

	size_t block_size;

	assert(TIO_L(t, ICANON));

	while ((block_size = ring_can_read(i_canon_ring, buflen, end - buff))) {
		const char *line_start = i_buff + i_canon_ring->tail;
		size_t line_len;
		int got_line;
		int is_eof;

		line_len = termios_find_line_len(t, line_start, block_size, &is_eof);

		got_line = (line_len < block_size);
		block_size = line_len;

		if (got_line) {
 			++block_size;  /* Line end char is always consumed... */

			if (!is_eof) { /* ...but EOF is discarded and not copied to user. */
	 			++line_len;
	 		}
		}

		memcpy(buff, line_start, line_len);
		buff += line_len;

		ring_just_read(i_ring, buflen, block_size);
		ring_just_read(i_canon_ring, buflen, block_size);

		if (got_line) {
			break;
		}
	}

	return buff;
}

char *termios_read(const struct termios *t, char *curr, char *end,
	struct ring *i_ring, struct ring *i_canon_ring, char *i_buff, size_t buflen) {

	struct ring raw_ring;
	size_t block_size;
	char *next = curr;

	while ((block_size = ring_can_read(
		termios_raw_ring(&raw_ring, i_ring, i_canon_ring), buflen, end - next))) {

		/* No processing is required to read raw data. */
		memcpy(curr, i_buff + raw_ring.tail, block_size);
		next += block_size;

		ring_just_read(i_ring, buflen, block_size);
	}

	if (TIO_L(t, ICANON)) {
		next = termios_read_cooked(t, next, end, 
			i_ring, i_canon_ring, i_buff, buflen);
	}

	return next;
}

void termios_update_size(const struct termios *t, size_t *size,
		unsigned long *timeout) {

	cc_t vmin = t->c_cc[VMIN];
	cc_t vtime = t->c_cc[VTIME];

	if (((vmin == 0) && (vtime == 0))) {
		*size = 0;
		*timeout = 0;
	} else {
		*size = vtime == 0 ? min(*size, vmin) : 1;
		*timeout = vmin > 0 ? SCHED_TIMEOUT_INFINITE
				: vtime * 100; /* deciseconds to milliseconds */
	}
}

void termios_update_ring(const struct termios *t, 
	struct ring *i_ring, struct ring *i_canon_ring) {
	
	if (!TIO_L(t, ICANON)) {
		i_canon_ring->tail = i_canon_ring->head = i_ring->head;
	}
}

int termios_can_read(const struct termios *t,
	struct ring *i_ring, struct ring *i_canon_ring, size_t buflen) {
	
	struct ring raw_ring;

	termios_raw_ring(&raw_ring, i_ring, i_canon_ring);

	return ring_can_read(&raw_ring, buflen, 1) ||
		(TIO_L(t, ICANON) && ring_can_read(i_canon_ring, buflen, 1));
}

void termios_init(struct termios *t) {
	cc_t cc_init[NCCS] = TERMIOS_CC_INIT;

	memcpy(t->c_cc, cc_init, sizeof(cc_init));

	t->c_iflag = TERMIOS_IFLAG_INIT;
	t->c_oflag = TERMIOS_OFLAG_INIT;
	t->c_cflag = TERMIOS_CFLAG_INIT;
	t->c_lflag = TERMIOS_LFLAG_INIT;
}
