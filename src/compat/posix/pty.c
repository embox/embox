/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    18.04.2014
 */

#include <pty.h>

extern int ppty(int pptyfds[2]);

int openpty(int *amaster, int *aslave, char *name, const struct termios *termp,
	       	const struct winsize *winp) {
	int fds[2];
	int ret;

	ret = ppty(fds);

	*amaster = fds[0];
	*aslave = fds[1];

	if (!ret && termp) {
		tcsetattr(fds[0], TCSANOW, termp);
	}
	return ret;
}
