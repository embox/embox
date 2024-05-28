/**
 * @file
 * @brief
 *
 * @date 1.09.22
 * @author Aleksey Zhmulin
 */

#include "curses_priv.h"

int bkgd(chtype ch) {
	return wbkgd(stdscr, ch);
}

void bkgdset(chtype ch) {
	wbkgdset(stdscr, ch);
}

void wbkgdset(WINDOW *win, chtype ch) {
	(void)win;
	(void)ch;
}

int wbkgd(WINDOW *win, chtype ch) {
	(void)win;
	(void)ch;

	return OK;
}
