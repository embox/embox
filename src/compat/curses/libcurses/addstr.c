/**
 * @file
 * @brief
 *
 * @date 1.09.22
 * @author Aleksey Zhmulin
 */

#include <stddef.h>
#include <string.h>

#include <util/math.h>

#include "curses_priv.h"

int addstr(const char *str) {
	return waddstr(stdscr, str);
}

int addnstr(const char *str, int n) {
	return waddnstr(stdscr, str, n);
}

int waddstr(WINDOW *win, const char *str) {
	return waddnstr(win, str, -1);
}

int waddnstr(WINDOW *win, const char *str, int n) {
	int res;
	curs_size_t str_len;

	if ((SP == NULL) || (win == NULL)) {
		return ERR;
	}

	if ((str == NULL) || ((n < 0) && (n != -1))) {
		return ERR;
	}

	str_len = strlen(str);
	n = (n == -1) ? str_len : min(n, str_len);

	while (n-- > 0) {
		res = waddch(win, *str++);
		if (res != OK) {
			return res;
		}
	}

	return OK;
}

int mvaddstr(int y, int x, const char *str) {
	return mvwaddstr(stdscr, y, x, str);
}

int mvaddnstr(int y, int x, const char *str, int n) {
	return mvwaddnstr(stdscr, y, x, str, n);
}

int mvwaddstr(WINDOW *win, int y, int x, const char *str) {
	return mvwaddnstr(win, y, x, str, -1);
}

int mvwaddnstr(WINDOW *win, int y, int x, const char *str, int n) {
	if (ERR == wmove(win, y, x)) {
		return ERR;
	}

	return waddnstr(win, str, n);
}
