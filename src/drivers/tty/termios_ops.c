/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    08.08.2013
 */

#include <ctype.h>
#include <termios.h>
#include <util/ring.h>

#include <drivers/termios_ops.h>

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
