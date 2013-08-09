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

#include <drivers/iodev.h>

extern int termios_putc(struct termios *tio, char ch, struct ring *ring, char *buf, size_t buflen);

static const struct termios diag_tio = {
	.c_lflag = ICANON,
	.c_oflag = ONLCR,
};

#define BUFLEN 4
static char buf[BUFLEN];
static struct ring r;

static int init_diag(void) {
	diag_init();
	return 0;
}

static const struct iodev_ops iodev_diag_ops_struct = {
	.init = &init_diag,
	.getc = &diag_getc,
	.putc = &diag_putc,
	.kbhit = &diag_kbhit,
};

static const struct iodev_ops *curr_iodev = &iodev_diag_ops_struct;

int iodev_init(void) {

	assert(curr_iodev->init);
	ring_init(&r);

	return curr_iodev->init();
}

char iodev_getc(void) {
	assert(curr_iodev->getc);
	return curr_iodev->getc();
}

void iodev_putc(char ch) {

	assert(curr_iodev->putc);

	termios_putc((struct termios *) &diag_tio, ch, &r, buf, BUFLEN);

	while (!ring_empty(&r)) {
		curr_iodev->putc(buf[r.tail]);
		ring_just_read(&r, BUFLEN, 1);
	}

}

int iodev_kbhit(void) {
	assert(curr_iodev->kbhit);
	return curr_iodev->kbhit();
}

#if 0
void iodev_setup(const struct iodev_ops *new_iodev) {
	curr_iodev = new_iodev;
}

struct iodev_ops const*iodev_current(void) {
	return curr_iodev;
}
#endif

