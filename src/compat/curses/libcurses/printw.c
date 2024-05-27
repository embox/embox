/**
 * @file
 * @brief
 *
 * @date 22.02.22
 * @author Aleksey Zhmulin
 */
#include <stdarg.h>
#include <stdio.h>

#include "curses_priv.h"

#define BUF_SZ 0x100

int mvwprintw(WINDOW *win, int y, int x, char *fmt, ...) {
	char buf[BUF_SZ];
	va_list args;
	int ret;

	ret = wmove(win, y, x);
	if (ret == OK) {
		va_start(args, fmt);
		vsprintf(buf, fmt, args);
		ret = waddstr(win, buf);
		va_end(args);
	}
	return ret;
}
