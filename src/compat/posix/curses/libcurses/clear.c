/**
 * @file
 * @brief
 *
 * @date 1.09.22
 * @author Aleksey Zhmulin
 */

#include <curses_priv.h>

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
