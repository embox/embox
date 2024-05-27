/**
 * @file
 * @brief
 *
 * @date 1.09.22
 * @author Aleksey Zhmulin
 */

#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <unistd.h>

#include "curses_priv.h"

int refresh(void) {
	if (SP == NULL) {
		return ERR;
	}

	return wrefresh(stdscr);
}

int wrefresh(WINDOW *win) {
	if ((SP == NULL) || (win == NULL)) {
		return ERR;
	}

	if (win == curscr) {
		curscr->flags |= WIN_CLEAROK;
	}
	else if (ERR == wnoutrefresh(win)) {
		return ERR;
	}

	return doupdate();
}

int wnoutrefresh(WINDOW *win) {
	curs_size_t src_line, dst_line;
	curs_size_t first, last;
	chtype *src, *dst;
	bool clearok = FALSE;

	if ((SP == NULL) || (win == NULL)) {
		return ERR;
	}

	if (!(win->flags & WIN_LEAVEOK)) {
		curscr->cury = win->cury + win->begy;
		curscr->curx = win->curx + win->begx;
	}

	if (win->flags & WIN_CLEAROK) {
		win->flags &= ~WIN_CLEAROK;
		if (win->flags & WIN_FULLWIN) {
			curscr->flags |= WIN_CLEAROK;
		}
		clearok = TRUE;
	}

	src_line = 0;
	dst_line = win->begy;

	for (; src_line < win->maxy; src_line++, dst_line++) {
		if (!clearok && (win->lines[src_line].firstchar == NOCHANGE)) {
			continue;
		}

		src = win->lines[src_line].text;
		dst = curscr->lines[dst_line].text + win->begx;

		if (clearok) {
			first = 0;
			last = win->maxx - 1;
		}
		else {
			first = win->lines[src_line].firstchar;
			last = win->lines[src_line].lastchar;

			while ((first <= last) && (src[first] == dst[first])) {
				first++;
			}

			while ((first <= last) && (src[last] == dst[last])) {
				last--;
			}
		}

		if (first <= last) {
			src += first;
			dst += first;
			memcpy(dst, src, (last - first + 1) * sizeof(chtype));

			first += win->begx;
			last += win->begx;

			if ((first < curscr->lines[dst_line].firstchar)
			    || (curscr->lines[dst_line].firstchar == NOCHANGE)) {
				curscr->lines[dst_line].firstchar = first;
			}

			if (last > curscr->lines[dst_line].lastchar) {
				curscr->lines[dst_line].lastchar = last;
			}
		}

		win->lines[src_line].firstchar = NOCHANGE;
		win->lines[src_line].lastchar = 0;
	}

	return OK;
}

int doupdate(void) {
	curs_size_t y;
	curs_size_t first, len;
	chtype *src, *dst;
	bool clearok = FALSE;

	if (SP == NULL) {
		return ERR;
	}

	if (isendwin()) {
		reset_prog_mode();
		_curs_putp(TINFO_ENTER_CA_MODE);
		SP->flags |= CURSES_MODE;
		curscr->flags |= WIN_CLEAROK;
	}

	if (curscr->flags & WIN_CLEAROK) {
		curscr->flags &= ~WIN_CLEAROK;
		clearok = TRUE;
	}

	for (y = 0; y < LINES; y++) {
		if (!clearok && (curscr->lines[y].firstchar == NOCHANGE)) {
			continue;
		}

		src = curscr->lines[y].text;
		dst = stdscr->lines[y].text;

		if (clearok) {
			first = 0;
			len = COLS;
		}
		else {
			first = curscr->lines[y].firstchar;
			len = curscr->lines[y].lastchar - first + 1;
		}

		src += first;
		dst += first;

		_curs_mvcur(y, first);
		_curs_puts(src, len);

		stdscr->cury = y;
		stdscr->curx = first + len;

		memcpy(dst, src, len * sizeof(chtype));

		curscr->lines[y].firstchar = NOCHANGE;
		curscr->lines[y].lastchar = 0;
	}

	if (!(curscr->flags & WIN_LEAVEOK)) {
		_curs_mvcur(curscr->cury, curscr->curx);
		stdscr->cury = curscr->cury;
		stdscr->curx = curscr->curx;
	}

	_curs_flush();

	return OK;
}

int wredrawln(WINDOW *win, int beg_line, int num_lines) {
	curs_size_t y;

	if (win == NULL) {
		return ERR;
	}

	for (y = beg_line; y < beg_line + num_lines; y++) {
		win->lines[y].firstchar = 0;
		win->lines[y].lastchar = win->maxx - 1;
	}
	return OK;
}
