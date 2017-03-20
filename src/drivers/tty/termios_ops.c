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

int termios_get_lflag(const struct termios *t, int flag) {
    return TIO_L(t, flag);
}

int termios_get_cc(const struct termios *t, int flag) {
    return t->c_cc[flag];
}

int termios_handle_newline(const struct termios *t, char ch, int *is_eol) {
    int ignore_cr = TIO_I(t, IGNCR) && ch == '\r';

    if (!ignore_cr) {
        if (TIO_I(t, ICRNL) && ch == '\r') ch = '\n';
        if (TIO_I(t, INLCR) && ch == '\n') ch = '\r';
    }
    *is_eol = (ch == '\n' || ch == t->c_cc[VEOL]);

    return ignore_cr;
}

int termios_handle_erase(const struct termios *t, char ch, int *erase_all) {
    *erase_all = (ch == t->c_cc[VKILL]);

    if (erase_all || ch == t->c_cc[VERASE] || ch == '\b') {
        return 1;
    }

    return 0;
}

int termios_check_end(const struct termios *t, char ch, int *is_eof) {
    cc_t eol = t->c_cc[VEOL];
    cc_t eof = t->c_cc[VEOF];

    if (ch == '\n' || ch == eol || ch == eof) {
        *is_eof = (ch == eof);
        return 1;
    }

    return 0;
}

size_t termios_get_size(const struct termios *t, size_t size, 
		unsigned long *timeout) {
    
    cc_t vmin = t->c_cc[VMIN];
    cc_t vtime = t->c_cc[VTIME];

    if (((vmin == 0) && (vtime == 0))) {
        size = 0;
        *timeout = 0;
    } else {
        size = vtime == 0 ? min(size, vmin) : 1;
        *timeout = vmin > 0 ? SCHED_TIMEOUT_INFINITE
                : vtime * 100; /* deciseconds to milliseconds */
    }

    return size;
}

void termios_init(struct termios *t) {
    cc_t cc_init[NCCS] = TTY_TERMIOS_CC_INIT;

    memcpy(t->c_cc, cc_init, sizeof(cc_init));
    t->c_iflag = TTY_TERMIOS_IFLAG_INIT;
    t->c_oflag = TTY_TERMIOS_OFLAG_INIT;
    t->c_cflag = TTY_TERMIOS_CFLAG_INIT;
    t->c_lflag = TTY_TERMIOS_LFLAG_INIT;
}