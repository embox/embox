/**
 * @file
 *
 * @date Mar 6, 2013
 * @author: Anton Bondarev
 */
#include <errno.h>
#include <string.h>
#include <kernel/irq.h>

#include <drivers/tty.h>
#include <kernel/thread/sched.h>

size_t tty_write(struct tty *tty, char *buff, size_t size) {
	return 0;
}

int tty_ioctl(struct tty *tty, int request, void *data) {
	switch (request) {
	case TIOCGETA:
		memcpy(&tty->termios, data, sizeof(struct termios));
		break;
	case TIOCSETA:
	case TIOCSETAF:
	case TIOCSETAW:
		memcpy(data, &tty->termios, sizeof(struct termios));
		break;
	default:
		return -ENOSYS;
	}

	return ENOERR;
}
