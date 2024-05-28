/**
 * @file
 * @brief
 *
 * @date 1.09.22
 * @author Aleksey Zhmulin
 */

#include <stddef.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "curses_priv.h"

#define INC_FIFO_ITER(it)        \
	if (++it == INPUT_FIFO_SIZE) \
	it = 0

static signed char input_fifo[INPUT_FIFO_SIZE];
static size_t input_fifo_beg;
static size_t input_fifo_end;

void input_fifo_push(int ch) {
	input_fifo[input_fifo_end] = ch;
	INC_FIFO_ITER(input_fifo_end);

	if (input_fifo_beg == input_fifo_end) {
		INC_FIFO_ITER(input_fifo_beg);
	}
}

int input_fifo_pull() {
	int ch = EOF;

	if (input_fifo_beg != input_fifo_end) {
		ch = input_fifo[input_fifo_beg];
		INC_FIFO_ITER(input_fifo_beg);
	}

	return ch;
}

int getch(void) {
	return wgetch(stdscr);
}

int wgetch(WINDOW *win) {
	int ch;
	signed char buf;
	bool reset_mode = FALSE;
	struct ctrl_seq *tmp = NULL;

	if ((SP == NULL) || (win == NULL)) {
		return ERR;
	}

	if (is_wintouched(win)) {
		wrefresh(win);
	}

	ch = input_fifo_pull();

	if ((win->flags & WIN_NODELAY) || (ch != EOF)) {
		return ch;
	}

	if (!(SP->flags & (RAW_MODE | CBREAK_MODE))) {
		cbreak();
		reset_mode = TRUE;
	}

	if (win->flags & WIN_KEYPAD) {
		tmp = _curs_ctrl_seq;
	}

	do {
		do {
			if (1 != read(fileno(SP->ifp), &buf, 1)) {
				ch = EOF;
			}
			else {
				ch = buf;
			}
			while (tmp && (ch != tmp->ch)) {
				tmp = tmp->sibling;
			}
			if (tmp && (tmp->value == 0)) {
				tmp = tmp->child;
				continue;
			}
			break;
		} while (1);

		if (tmp) {
			break;
		}

		input_fifo_push(ch);

		if (SP->flags & ECHO_MODE) {
			mvwaddch(win, win->cury, win->curx, ch);
			wrefresh(win);
		}
	} while ((reset_mode && (ch != '\n')));

	if (reset_mode) {
		nocbreak();
	}

	if (tmp) {
		return tmp->value;
	}

	return input_fifo_pull();
}

int mvgetch(int y, int x) {
	return mvwgetch(stdscr, y, x);
}

int mvwgetch(WINDOW *win, int y, int x) {
	int res;

	res = wmove(win, y, x);
	if (res != OK) {
		return res;
	}

	return wgetch(win);
}

int ungetch(int ch) {
	return ungetc(ch, SP->ifp);
}
