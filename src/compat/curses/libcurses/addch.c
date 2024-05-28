/**
 * @file
 * @brief
 *
 * @date 1.09.22
 * @author Aleksey Zhmulin
 */

#include <stddef.h>
#include <string.h>

#include "curses_priv.h"

int addch(chtype ch) {
	return waddch(stdscr, ch);
}

int waddch(WINDOW *win, chtype ch) {
	curs_size_t y, x;

	if (win == NULL) {
		return ERR;
	}

	y = win->cury;
	x = win->curx;

	switch (ch) {
	case '\r':
		x = 0;
		break;

	default:
		if ((ch < 32) || (ch > 126)) {
			return ERR;
		}
		if (ch != win->lines[y].text[x]) {
			if ((x < win->lines[y].firstchar)
			    || (win->lines[y].firstchar == NOCHANGE)) {
				win->lines[y].firstchar = x;
			}
			if (x > win->lines[y].lastchar) {
				win->lines[y].lastchar = x;
			}
			win->lines[y].text[x] = ch;
		}
		if (++x >= win->maxx) {
			x = 0;
			if (y < win->maxy - 1) {
				y++;
			}
		}
	}

	win->cury = y;
	win->curx = x;

	return OK;
}

int mvaddch(int y, int x, chtype ch) {
	return mvwaddch(stdscr, y, x, ch);
}

int mvwaddch(WINDOW *win, int y, int x, chtype ch) {
	if (ERR == wmove(win, y, x)) {
		return ERR;
	}

	return waddch(win, ch);
}
