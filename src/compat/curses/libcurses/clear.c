/**
 * @file
 * @brief
 *
 * @date 1.09.22
 * @author Aleksey Zhmulin
 */

#include <stddef.h>

#include "curses_priv.h"

int erase(void) {
	return clear();
}

int werase(WINDOW *win) {
	return wclear(win);
}

int clear(void) {
	return wclear(stdscr);
}

int wclear(WINDOW *win) {
	return clearok(win, TRUE);
}

int wclrtoeol(WINDOW *win) {
	curs_size_t y, x;

	if (win == NULL) {
		return ERR;
	}

	y = win->cury;
	x = win->curx;

	if (x < win->lines[y].firstchar) {
		win->lines[y].firstchar = x;
	}
	win->lines[y].lastchar = win->maxx - 1;

	for (; x < win->maxx; x++) {
		win->lines[y].text[x] = ' ';
	}
	return OK;
}
