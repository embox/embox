/**
 * @file
 * @brief
 *
 * @date 1.09.22
 * @author Aleksey Zhmulin
 */

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "curses_priv.h"

static int init_window(WINDOW *win, curs_size_t nlines, curs_size_t ncols,
    curs_size_t begy, curs_size_t begx, WINDOW *parent) {
	int i;

	win->parent = parent;
	win->maxy = nlines;
	win->maxx = ncols;
	win->begy = begy;
	win->begx = begx;
	win->curx = 0;
	win->curx = 0;

	if ((nlines == LINES) && (ncols == COLS)) {
		win->flags |= WIN_FULLWIN;
	}
	win->flags |= WIN_CLEAROK;
	win->flags &= ~WIN_NODELAY;
	win->flags &= ~WIN_LEAVEOK;

	win->lines = (struct line *)malloc(nlines * sizeof(struct line));
	if (win->lines == NULL) {
		return ERR;
	}

	win->lines[0].text = (chtype *)malloc(nlines * ncols * sizeof(chtype));
	if (win->lines[0].text == NULL) {
		free(win->lines);
		return ERR;
	}

	memset(win->lines[0].text, ' ', nlines * ncols * sizeof(chtype));

	for (i = 0; i < nlines; i++) {
		win->lines[i].text = win->lines[0].text + ncols * i;
		win->lines[i].firstchar = 0;
		win->lines[i].lastchar = ncols - 1;
	}

	return OK;
}

WINDOW *newwin(int nlines, int ncols, int begy, int begx) {
	WINDOW *win;

	if (SP == NULL) {
		return NULL;
	}

	if ((nlines <= 0) || (ncols <= 0) || (begy < 0) || (begx < 0)
	    || (begy + nlines > LINES) || (begx + ncols > COLS)) {
		return NULL;
	}

	win = malloc(sizeof(WINDOW));
	if (win != NULL) {
		if (ERR == init_window(win, nlines, ncols, begy, begx, NULL)) {
			free(win);
			win = NULL;
		}
	}

	return win;
}

int delwin(WINDOW *win) {
	if ((SP == NULL) || (win == NULL)) {
		return ERR;
	}

	free(win->lines[0].text);
	free(win->lines);
	free(win);

	return OK;
}

WINDOW *subwin(WINDOW *orig, int nlines, int ncols, int begy, int begx) {
	WINDOW *win;

	if ((SP == NULL) || (orig == NULL)) {
		return NULL;
	}

	if ((nlines <= 0) || (ncols <= 0) || (begy < 0) || (begx < 0)
	    || (begy + nlines > LINES) || (begx + ncols > COLS)) {
		return NULL;
	}

	win = malloc(sizeof(WINDOW));
	if (win == NULL) {
		return NULL;
	}

	if (ERR == init_window(win, nlines, ncols, begy, begx, orig)) {
		return NULL;
	}

	return win;
}

WINDOW *derwin(WINDOW *orig, int nlines, int ncols, int begy, int begx) {
	return subwin(orig, nlines, ncols, orig->begy + begy, orig->begx + begx);
}
