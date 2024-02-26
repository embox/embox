/**
 * @file
 *
 * @date Feb 28, 2013
 * @author: Anton Bondarev
 */

#include <errno.h>
#include <posix_errno.h>
#include <stropts.h>
#include <sys/ioctl.h>
#include <termios.h>

#include <drivers/tty.h>

speed_t cfgetospeed(const struct termios *termios) {
	return termios->c_cflag & B38400;
}

speed_t cfgetispeed(const struct termios *termios) {
	return cfgetospeed(termios);
}

int cfsetospeed(struct termios *termios, speed_t speed) {
	if (speed & ~B38400) {
		return SET_ERRNO(EINVAL);
	}

	termios->c_cflag &= ~B38400;
	termios->c_cflag |= speed;

	return 0;
}

int cfsetispeed(struct termios *termios, speed_t speed) {
	return cfsetospeed(termios, speed);
}

int tcgetattr(int fd, struct termios *termios) {
	return ioctl(fd, TCGETS, termios);
}

int tcsetattr(int fd, int opt, const struct termios *termios) {
	switch (opt) {
	case TCSANOW:
		return (ioctl(fd, TCSETS, termios));
	case TCSADRAIN:
		return (ioctl(fd, TCSETSW, termios));
	case TCSAFLUSH:
		return (ioctl(fd, TCSETSF, termios));
	default:
		errno = EINVAL;
		return (-1);
	}
}

pid_t tcgetpgrp(int fd) {
	pid_t pid;

	if (-1 == ioctl(fd, TIOCGPGRP, &pid)) {
		return -1;
	}

	return pid;
}

int tcsetpgrp(int fd, pid_t pgrp) {
	return ioctl(fd, TIOCSPGRP, &pgrp);
}

int tcflush(int fd, int queue_selector) {
	int arg;

	switch (queue_selector) {
	case TCIFLUSH:
		arg = FLUSHR;
		break;
	case TCOFLUSH:
		arg = FLUSHW;
		break;
	case TCIOFLUSH:
		arg = FLUSHRW;
		break;
	default:
		SET_ERRNO(EINVAL);
		return -1;
	}

	return ioctl(fd, I_FLUSH, (void *)&arg);
}
