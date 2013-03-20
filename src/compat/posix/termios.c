/**
 * @file
 *
 * @date Feb 28, 2013
 * @author: Anton Bondarev
 */
#include <sys/ioctl.h>
#include <errno.h>
#include <drivers/tty.h>
#include <termios.h>


int tcgetattr(int fd, struct termios *termios_p) {
	return ioctl(fd, TTY_IOCTL_GETATTR, termios_p);
}

int tcsetattr(int fd, int optional_actions, struct termios *termios_p) {
	return ioctl(fd, TTY_IOCTL_SETATTR, termios_p);
}
