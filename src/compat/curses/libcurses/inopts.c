/**
 * @file
 * @brief
 *
 * @date 1.09.22
 * @author Aleksey Zhmulin
 */

#include <stdbool.h>
#include <stddef.h>
#include <termios.h>

#include "curses_priv.h"

int cbreak(void) {
	if (SP == NULL) {
		return ERR;
	}

	SP->prog_tty.c_iflag &= ~ICRNL;
	SP->prog_tty.c_lflag &= ~ICANON;
	SP->prog_tty.c_lflag |= ISIG;
	SP->prog_tty.c_cc[VMIN] = 1;
	SP->prog_tty.c_cc[VTIME] = 0;

	reset_prog_mode();

	SP->flags |= CBREAK_MODE;

	return OK;
}

int nocbreak(void) {
	if (SP == NULL) {
		return ERR;
	}

	SP->prog_tty.c_iflag |= ICRNL;
	SP->prog_tty.c_lflag |= ICANON;

	reset_prog_mode();

	SP->flags &= ~CBREAK_MODE;

	return OK;
}

int keypad(WINDOW *win, bool bf) {
	if ((SP == NULL) || (win == NULL)) {
		return ERR;
	}

	if (bf) {
		win->flags |= WIN_KEYPAD;
	}
	else {
		win->flags &= ~WIN_KEYPAD;
	}

	return OK;
}

int nodelay(WINDOW *win, bool bf) {
	if ((SP == NULL) || (win == NULL)) {
		return ERR;
	}

	if (bf) {
		win->flags |= WIN_NODELAY;
	}
	else {
		win->flags &= ~WIN_NODELAY;
	}

	return OK;
}

int echo(void) {
	if (SP == NULL) {
		return ERR;
	}

	SP->flags |= ECHO_MODE;

	return OK;
}

int noecho(void) {
	if (SP == NULL) {
		return ERR;
	}

	SP->flags &= ~ECHO_MODE;

	return OK;
}

int raw(void) {
	if (SP == NULL) {
		return ERR;
	}

	SP->prog_tty.c_iflag &= ~(IXON | BRKINT | PARMRK);
	SP->prog_tty.c_lflag &= ~(ICANON | ISIG | IEXTEN);
	SP->prog_tty.c_cc[VMIN] = 1;
	SP->prog_tty.c_cc[VTIME] = 0;

	reset_prog_mode();

	SP->flags |= RAW_MODE;
	SP->flags |= CBREAK_MODE;

	return OK;
}

int noraw(void) {
	if (SP == NULL) {
		return ERR;
	}

	SP->prog_tty.c_iflag |= (IXON | BRKINT | PARMRK);
	SP->prog_tty.c_lflag |= (ISIG | ICANON
	                         | (_curs_shell_tty.c_lflag & IEXTEN));

	reset_prog_mode();

	SP->flags &= ~RAW_MODE;
	SP->flags &= ~CBREAK_MODE;

	return OK;
}
