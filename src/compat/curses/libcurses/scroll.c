/**
 * @file
 * @brief
 *
 * @date 1.09.22
 * @author Aleksey Zhmulin
 */

#include "curses_priv.h"

int scroll(WINDOW *win) {
	return wscrl(win, 1);
}

int scrl(int n) {
	return wscrl(stdscr, n);
}

int wscrl(WINDOW *win, int n) {
	(void)win;
	(void)n;

	return OK;
}