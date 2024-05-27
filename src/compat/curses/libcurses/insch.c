/**
 * @file
 * @brief
 *
 * @date 1.09.22
 * @author Aleksey Zhmulin
 */

#include <stddef.h>

#include "curses_priv.h"

int insch(chtype ch) {
	return winsch(stdscr, ch);
}

int winsch(WINDOW *win, chtype ch) {
	if (win == NULL) {
		return ERR;
	}

	return mvwinsch(win, win->cury, win->curx, ch);
}

int mvinsch(int y, int x, chtype ch) {
	return mvwinsch(stdscr, y, x, ch);
}

int mvwinsch(WINDOW *win, int y, int x, chtype ch) {
	if ((SP == NULL) || (win == NULL)) {
		return ERR;
	}

	if (ERR == mvwaddch(win, y, x, ch)) {
		return ERR;
	}

	win->cury = y;
	win->curx = x;

	return OK;
}
