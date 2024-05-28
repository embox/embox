/**
 * @file
 * @brief
 *
 * @date 1.09.22
 * @author Aleksey Zhmulin
 */

#include "curses_priv.h"

int attroff(int attrs) {
	return wattroff(stdscr, attrs);
}

int wattroff(WINDOW *win, int attrs) {
	(void)win;
	(void)attrs;

	return OK;
}

int attron(int attrs) {
	return wattron(stdscr, attrs);
}

int wattron(WINDOW *win, int attrs) {
	(void)win;
	(void)attrs;

	return OK;
}

int attrset(int attrs) {
	return wattrset(stdscr, attrs);
}

int wattrset(WINDOW *win, int attrs) {
	(void)win;
	(void)attrs;

	return OK;
}
