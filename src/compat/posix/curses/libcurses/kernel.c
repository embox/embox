/**
 * @file
 * @brief
 *
 * @date 1.09.22
 * @author Aleksey Zhmulin
 */

#include <curses_priv.h>

#include <time.h>

int def_prog_mode(void) {
	return _curs_get_tty_mode(&SP->prog_tty);
}

int def_shell_mode(void) {
	return _curs_get_tty_mode(&_curs_shell_tty);
}

int reset_prog_mode(void) {
	return _curs_set_tty_mode(&SP->prog_tty);
}

int reset_shell_mode(void) {
	return _curs_set_tty_mode(&_curs_shell_tty);
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
	}

	if (rc != ERR) {
		rc = _curs_flush();
	}

	return rc;
}

int napms(int ms) {
	struct timespec ts;

	ts.tv_sec = ms / 1000;
	ts.tv_nsec = (ms % 1000) * 1000000;
	nanosleep(&ts, NULL);
	return OK;
}
