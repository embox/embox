/**
 * @file
 * @brief
 *
 * @date 05.02.13
 * @author Ilia Vaprol
 * @author Anton Kozlov
 */

#include <errno_codes.h>
#include <stddef.h>
#include <kernel/panic.h>
#include <util/ring.h>
#include <drivers/diag.h>
#include <drivers/tty/termios_ops.h>
#include <termios.h>
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
	const struct diag *tdiag = cdiag;

	if (tdiag->ops->init) {
		return tdiag->ops->init(tdiag);
	}

	return 0;
}

char diag_getc(void) {
	const struct diag *tdiag = cdiag;
	char ch;

	if (!tdiag->ops->getc) {
		panic("diag_getc called with no implementation");
	}

	if (tdiag->ops->kbhit) {
		while (!tdiag->ops->kbhit(tdiag)) {

		}
	}

	ch = tdiag->ops->getc(tdiag);
	return ch != '\r' ? ch : '\n';
}

#define BUFLEN 4
void diag_putc(char ch) {
	char buf[BUFLEN];
	struct ring r;
	const struct diag *tdiag = cdiag;

	assert(tdiag->ops->putc);

	ring_init(&r);

	termios_putc(&diag_tio, ch, &r, buf, BUFLEN);

	while (!ring_empty(&r)) {
		tdiag->ops->putc(tdiag, buf[r.tail]);
		ring_just_read(&r, BUFLEN, 1);
	}
}

extern enum diag_kbhit_ret diag_kbhit(void) {
	const struct diag *tdiag = cdiag;

	if (tdiag->ops->kbhit) {
		return tdiag->ops->kbhit(tdiag) ? KBHIT_CAN_GETC : KBHIT_WILL_BLK;
	}

	if (tdiag->ops->getc) {
		return KBHIT_WILL_BLK;
	}

	return KBHIT_WILL_FOREVER;
}

int diag_setup(const struct diag *diag) {

	if (!diag) {
		return -EINVAL;
	}

	cdiag = diag;

	return 0;
}
#if 0
void iodev_setup(const struct iodev_ops *new_iodev) {
	cdiag = new_iodev;
}

struct iodev_ops const*iodev_current(void) {
	return cdiag;
}
#endif

