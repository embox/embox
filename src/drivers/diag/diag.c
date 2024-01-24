/**
 * @file
 * @brief
 *
 * @date 05.02.13
 * @author Ilia Vaprol
 * @author Anton Kozlov
 */

#include <errno.h>
#include <stddef.h>
#include <termios.h>

#include <kernel/panic.h>
#include <lib/libds/ring.h>
#include <drivers/diag.h>
#include <drivers/tty/termios_ops.h>

#include <framework/mod/options.h>

#if OPTION_DEFINED(STRING,impl)
#define DIAG_IMPL DIAG_IMPL_NAME(OPTION_GET(STRING,impl))
#else
#error No impl option provided
#endif

extern const struct diag DIAG_IMPL;
static const struct diag *cdiag = &DIAG_IMPL;

static const struct termios diag_tio = {
	.c_lflag = ICANON,
	.c_oflag = ONLCR,
};

int diag_init(void) {

	if (cdiag->ops->init) {
		return cdiag->ops->init(cdiag);
	}

	return 0;
}

char diag_getc(void) {
	char ch;

	if (!cdiag->ops->getc) {
		panic("diag_getc called with no implementation");
	}

	if (cdiag->ops->kbhit) {
		while (!cdiag->ops->kbhit(cdiag)) {
		}

		ch = cdiag->ops->getc(cdiag);
	} else {

		do {
			ch = cdiag->ops->getc(cdiag);
		} while (ch == (char)-1);
	}

	return ch;
}

#define BUFLEN 4
void diag_putc(char ch) {
	char buf[BUFLEN];
	struct ring r;

	assert(cdiag->ops->putc);

	ring_init(&r);

	termios_putc(&diag_tio, ch, &r, buf, BUFLEN);

	while (!ring_empty(&r)) {
		cdiag->ops->putc(cdiag, buf[r.tail]);
		ring_just_read(&r, BUFLEN, 1);
	}
}

int diag_kbhit(void) {

	if (cdiag->ops->kbhit) {
		return cdiag->ops->kbhit(cdiag);
	}

	return 0;
}
