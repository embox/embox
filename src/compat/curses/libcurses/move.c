/**
 * @file
 * @brief
 *
 * @date 1.09.22
 * @author Aleksey Zhmulin
 */

#include <stddef.h>

#include "curses_priv.h"

int move(int y, int x) {
	return wmove(stdscr, y, x);
}

int wmove(WINDOW *win, int y, int x) {
	if ((SP == NULL) || (win == NULL)) {
		return ERR;
	}

	if ((y < 0) || (x < 0) || (y >= win->maxy) || (x >= win->maxx)) {
		return ERR;
	}

	win->cury = y;
	win->curx = x;

	return OK;
}
