/**
 * @file
 * @brief
 *
 * @date 1.09.22
 * @author Aleksey Zhmulin
 */

#include <stddef.h>

#include "curses_priv.h"

int delch(void) {
	return wdelch(stdscr);
}

int wdelch(WINDOW *win) {
	if (win == NULL) {
		return ERR;
	}

	return mvwdelch(win, win->cury, win->curx);
}

int mvdelch(int y, int x) {
	return mvwdelch(stdscr, y, x);
}

int mvwdelch(WINDOW *win, int y, int x) {
	if ((SP == NULL) || (win == NULL)) {
		return ERR;
	}

	if (ERR == mvwaddch(win, y, x, ' ')) {
		return ERR;
	}

	win->cury = y;
	win->curx = x;

	return OK;
}
