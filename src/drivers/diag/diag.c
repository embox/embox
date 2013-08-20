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
#include <kernel/panic.h>
#include <util/ring.h>
#include <drivers/diag.h>
#include <termios.h>
#include <framework/mod/options.h>

#if OPTION_DEFINED(STRING,impl)
#define DIAG_IMPL DIAG_IMPL_NAME(OPTION_GET(STRING,impl))
#else
#error No impl option provided
#endif

extern int termios_putc(struct termios *tio, char ch, struct ring *ring, char *buf, size_t buflen);

extern const struct diag DIAG_IMPL;
static const struct diag *cdiag = &DIAG_IMPL;

#define BUFLEN 4

static const struct termios diag_tio = {
	.c_lflag = ICANON,
	.c_oflag = ONLCR,
};

static char buf[BUFLEN];
static struct ring r;

int diag_init(void) {

	ring_init(&r);

	if (cdiag->ops->init) {
		return cdiag->ops->init(cdiag);
	}

	return 0;
}

char diag_getc(void) {

	if (!cdiag->ops->getc) {
		panic("diag_getc called with no implementation");
	}

	if (cdiag->ops->kbhit) {
		while (!cdiag->ops->kbhit(cdiag)) {

		}
	}

	return cdiag->ops->getc(cdiag);
}

void diag_putc(char ch) {

	assert(cdiag->ops->putc);

	termios_putc((struct termios *) &diag_tio, ch, &r, buf, BUFLEN);

	while (!ring_empty(&r)) {
		cdiag->ops->putc(cdiag, buf[r.tail]);
		ring_just_read(&r, BUFLEN, 1);
	}
}

extern enum diag_kbhit_ret diag_kbhit(void) {

	if (cdiag->ops->kbhit) {
		return cdiag->ops->kbhit(cdiag) ? KBHIT_CAN_GETC : KBHIT_WILL_BLK;
	}

	if (cdiag->ops->getc) {
		return KBHIT_WILL_BLK;
	}

	return KBHIT_WILL_FOREVER;
}

#if 0
void iodev_setup(const struct iodev_ops *new_iodev) {
	cdiag = new_iodev;
}

struct iodev_ops const*iodev_current(void) {
	return cdiag;
}
#endif

