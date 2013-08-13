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
#include <util/ring.h>
#include <drivers/diag.h>
#include <termios.h>
#include <framework/mod/options.h>

#if OPTION_DEFINED(STRING,impl)
#define DIAG_OPS DIAG_OPS_NAME(OPTION_GET(STRING,impl))
#else
#error No diag_ops option provided
#endif

extern int termios_putc(struct termios *tio, char ch, struct ring *ring, char *buf, size_t buflen);

extern const struct diag_ops DIAG_OPS;
static const struct diag_ops *cdiag = &DIAG_OPS;

#define BUFLEN 4

static const struct termios diag_tio = {
	.c_lflag = ICANON,
	.c_oflag = ONLCR,
};

static char buf[BUFLEN];
static struct ring r;

int diag_init(void) {

	ring_init(&r);

	if (cdiag->init) {
		return cdiag->init();
	}

	return 0;
}

char diag_getc(void) {

	if (cdiag->getc) {
		return cdiag->getc();
	}

	return '\0';
}

void diag_putc(char ch) {

	assert(cdiag->putc);

	termios_putc((struct termios *) &diag_tio, ch, &r, buf, BUFLEN);

	while (!ring_empty(&r)) {
		cdiag->putc(buf[r.tail]);
		ring_just_read(&r, BUFLEN, 1);
	}
}

int diag_kbhit(void) {

	if (cdiag->kbhit) {
		return cdiag->kbhit();
	}

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

