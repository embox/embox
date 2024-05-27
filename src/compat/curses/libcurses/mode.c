/**
 * @file
 * @brief
 *
 * @date 1.09.22
 * @author Aleksey Zhmulin
 */

#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <termios.h>

#include "curses_priv.h"

static int get_tty_mode(struct termios *t) {
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

static int set_tty_mode(struct termios *t) {
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

int def_prog_mode(void) {
	return get_tty_mode(&SP->prog_tty);
}

int def_shell_mode(void) {
	return get_tty_mode(&_curs_shell_tty);
}

int reset_prog_mode(void) {
	return set_tty_mode(&SP->prog_tty);
}

int reset_shell_mode(void) {
	return set_tty_mode(&_curs_shell_tty);
}

int curs_set(int visibility) {
	int rc;

	if (SP == NULL) {
		return ERR;
	}

	switch (visibility) {
	case 0:
		rc = _curs_putp(TINFO_CURSOR_INVISIBLE);
		break;
	case 1:
		rc = _curs_putp(TINFO_CURSOR_NORMAL);
		break;
	case 2:
		rc = _curs_putp(TINFO_CURSOR_VISIBLE);
		break;
	default:
		rc = ERR;
		break;
	}

	if (rc != ERR) {
		rc = _curs_flush();
	}

	return rc;
}
