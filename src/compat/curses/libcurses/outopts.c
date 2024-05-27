/**
 * @file
 * @brief
 *
 * @date 1.09.22
 * @author Aleksey Zhmulin
 */

#include <stdbool.h>
#include <stddef.h>

#include "curses_priv.h"

int clearok(WINDOW *win, bool bf) {
	if ((SP == NULL) || (win == NULL)) {
		return ERR;
	}

	if (bf) {
		win->flags |= (WIN_CLEAROK);
	}
	else {
		win->flags &= ~(WIN_CLEAROK);
	}

	return OK;
}

int leaveok(WINDOW *win, bool bf) {
	if ((SP == NULL) || (win == NULL)) {
		return ERR;
	}

	if (bf) {
		win->flags |= (WIN_LEAVEOK);
	}
	else {
		win->flags &= ~(WIN_LEAVEOK);
	}

	return OK;
}

int scrollok(WINDOW *win, bool bf) {
	if ((SP == NULL) || (win == NULL)) {
		return ERR;
	}

	if (bf) {
		win->flags |= (WIN_SCROLLOK);
	}
	else {
		win->flags &= ~(WIN_SCROLLOK);
	}

	return OK;
}

int nl(void) {
	if (SP == NULL) {
		return ERR;
	}

	SP->flags |= (NL_MODE);

	return OK;
}

int nonl(void) {
	if (SP == NULL) {
		return ERR;
	}

	SP->flags &= ~(NL_MODE);

	return OK;
}
