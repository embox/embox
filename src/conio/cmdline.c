/*
 * cmdline.c
 *
 *  Created on: 08.02.2009
 *      Author: Eldar Abusalimov
 */

#include "cmdline.h"

#include "string.h"
#include "common.h"

void cmdline_init(CMDLINE *cmdline) {
	CMDLINE_HISTORY * history = &(cmdline->history);
	history->array[0][0] = '\0';
	history->array[1][0] = '\0';
	history->index = 0;
	cmdline->string[0] = '\0';
	cmdline->length = 0;
	cmdline->cursor = 0;
	cmdline->history_cursor = 0;
}

BOOL cmdline_history_move_to(CMDLINE *cmdline, int to) {
	unsigned int new_pos;
	CMDLINE_HISTORY * history = &(cmdline->history);
	if (to < 0) {
		to = to + (-to / CMDLINE_HISTORY_SIZE + 1) * CMDLINE_HISTORY_SIZE;
	}
	new_pos = to % CMDLINE_HISTORY_SIZE;
	if (cmdline->history_cursor == new_pos) {
		return FALSE;
	}
	if (history->array[new_pos][0] == '\0' && new_pos != history->index) {
		return FALSE;
	}

	if (cmdline->history_cursor == history->index) {
		sz_cpy(history->array[history->index], cmdline->string);
	}

	cmdline->history_cursor = new_pos;
	sz_cpy(cmdline->string, history->array[new_pos]);
	cmdline->length = sz_length(cmdline->string);
	cmdline->cursor = cmdline->length;
	return TRUE;
}

inline BOOL cmdline_history_move_by(CMDLINE *cmdline, int by) {
	return cmdline_history_move_to(cmdline, cmdline->history_cursor + by);
}

inline BOOL cmdline_history_backward(CMDLINE *cmdline) {
	return cmdline_history_move_by(cmdline, -1);
}

inline BOOL cmdline_history_forward(CMDLINE *cmdline) {
	return cmdline_history_move_by(cmdline, 1);
}

BOOL cmdline_history_new(CMDLINE *cmdline) {
	CMDLINE_HISTORY * history = &(cmdline->history);
	if (cmdline->string[0] == '\0') {
		return FALSE;
	}

	sz_cpy(history->array[history->index], cmdline->string);

	history->index = (history->index + 1) % CMDLINE_HISTORY_SIZE;
	history->array[history->index][0] = '\0';
	history->array[(history->index + 1) % CMDLINE_HISTORY_SIZE][0] = '\0';

	cmdline->history_cursor = history->index;
	cmdline->length = 0;
	cmdline->cursor = 0;
	cmdline->string[0] = '\0';

	return TRUE;
}

BOOL cmdline_cursor_move_to(CMDLINE *cmdline, unsigned int to) {
	unsigned int old_cursor = cmdline->cursor;
	cmdline->cursor = min(to, cmdline->length);
	return cmdline->cursor != old_cursor;
}

inline BOOL cmdline_cursor_move_by(CMDLINE *cmdline, int by) {
	return cmdline_cursor_move_to(cmdline, cmdline->cursor + by);
}

inline BOOL cmdline_cursor_right(CMDLINE *cmdline) {
	return cmdline_cursor_move_by(cmdline, 1);
}

inline BOOL cmdline_cursor_left(CMDLINE *cmdline) {
	return cmdline_cursor_move_by(cmdline, -1);
}

inline BOOL cmdline_cursor_home(CMDLINE *cmdline) {
	return cmdline_cursor_move_to(cmdline, 0);
}

inline BOOL cmdline_cursor_end(CMDLINE *cmdline) {
	return cmdline_cursor_move_to(cmdline, cmdline->length);
}

BOOL cmdline_char_delete(CMDLINE *cmdline) {
	int i;
	if (cmdline->cursor == cmdline->length) {
		return FALSE;
	}

	for (i = cmdline->cursor; i < cmdline->length; ++i) {
		cmdline->string[i] = cmdline->string[i + 1];
	}
	cmdline->length--;

	return TRUE;
}

BOOL cmdline_char_backspace(CMDLINE *cmdline) {
	if (cmdline->cursor == 0) {
		return FALSE;
	}
	cmdline->cursor--;

	return cmdline_char_delete(cmdline);
}

BOOL cmdline_char_insert(CMDLINE *cmdline, char ch) {
	int i;
	if (cmdline->length == CMDLINE_MAX_LENGTH) {
		return FALSE;
	}

	for (i = cmdline->length; i > cmdline->cursor; --i) {
		cmdline->string[i] = cmdline->string[i - 1];
	}
	cmdline->string[cmdline->cursor++] = ch;
	cmdline->string[++cmdline->length] = '\0';

	return TRUE;
}

