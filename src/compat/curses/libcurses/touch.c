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

bool is_wintouched(WINDOW *win) {
	WINDOW *win2;
	int i;

	if ((SP == NULL) || (win == NULL)) {
		return FALSE;
	}

	for (i = 0; i < win->maxy; i++) {
		if (win->lines[i].firstchar != NOCHANGE)
			return TRUE;
	}

	if (!(win->flags & WIN_LEAVEOK)) {
		win2 = (win == stdscr) ? curscr : stdscr;
		if ((win->cury != win2->cury + win->begy)
		    || (win->curx != win2->curx + win->begx)) {
			return TRUE;
		}
	}

	return FALSE;
}
