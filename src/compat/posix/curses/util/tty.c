/**
 * @file
 * @brief
 *
 * @date 1.09.22
 * @author Aleksey Zhmulin
 */

#include <curses_priv.h>

#include <errno.h>
#include <stdio.h>
#include <termios.h>

int _curs_get_tty_mode(struct termios *t) {
	if (SP == NULL) {
		return ERR;
	}

	while (-1 == tcgetattr(fileno(SP->ifp), t)) {
		if ((errno != EINTR) && (errno != EAGAIN)) {
			return ERR;
		}
	}

	return OK;
}

int _curs_set_tty_mode(struct termios *t) {
	if (SP == NULL) {
		return ERR;
	}

	while (-1 == tcsetattr(fileno(SP->ifp), TCSANOW, t)) {
		if ((errno != EINTR) && (errno != EAGAIN)) {
			return ERR;
		}
	}

	return OK;
}
