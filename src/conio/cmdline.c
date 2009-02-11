/*
 * cmdline.c
 *
 *  Created on: 08.02.2009
 *      Author: Eldar Abusalimov
 */

#include "cmdline.h"

#include "string.h"
#include "common.h"

#define CMDLINE_MAX_LENGTH		63
#define HISTORY_SIZE	8

static char history[HISTORY_SIZE][CMDLINE_MAX_LENGTH + 1];
static int history_index = 0;

BOOL cmdline_init(CMDLINE *cmdline) {
	cmdline->string = history[0];
	cmdline->string[0] = '\0';
	cmdline->length = 0;
	cmdline->cursor = 0;
	cmdline->history_pos = history_index;
}

BOOL cmdline_history_move_to(CMDLINE *cmdline, int to) {
	int new_pos = to % HISTORY_SIZE;
	if (history[new_pos][0] == '\0' && new_pos != history_index) {
		return FALSE;
	}

	cmdline->history_pos = new_pos;
	sz_cpy(cmdline->string, history[new_pos]);
	cmdline->length = sz_length(cmdline->string);
	cmdline->cursor = cmdline->length;
	return TRUE;
}

inline BOOL cmdline_history_move_by(CMDLINE *cmdline, int by) {
	return cmdline_history_move_to(cmdline, cmdline->history_pos + by);
}

inline BOOL cmdline_history_backward(CMDLINE *cmdline) {
	return cmdline_history_move_by(cmdline, -1);
}

inline BOOL cmdline_history_forward(CMDLINE *cmdline) {
	return cmdline_history_move_by(cmdline, 1);
}

BOOL cmdline_history_new(CMDLINE *cmdline) {
	history_index = (history_index + 1) % HISTORY_SIZE;
	cmdline->string = history[history_index];

	cmdline->history_pos = history_index;
	cmdline->length = 0;
	cmdline->cursor = cmdline->length;
	cmdline->string[cmdline->length] = '\0';
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

