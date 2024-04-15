/**
 * @file
 * @brief Internal representation of command line entity.
 *
 * @date 08.02.09
 * @author Eldar Abusalimov
 */

#include "cmdline.h"

#include <string.h>

#include <util/math.h>
#include <lib/libds/array.h>

CMDLINE * cmdline_init(CMDLINE *this) {
	CMDLINE_HISTORY *history;
	if (this == NULL) {
		return NULL;
	}

	history = this->history;
	history->array[0][0] = '\0';
#if CMDLINE_HISTORY_SIZE > 1
	history->array[1][0] = '\0';
#endif
	history->index = 0;
	this->string[0] = '\0';
	this->length = 0;
	this->cursor = 0;
	this->history_cursor = 0;
	this->is_insert_mode = 0;
	return this;
}

static bool cmdline_history_move_to(CMDLINE *cmdline, int to) {
	int new_pos;
	CMDLINE_HISTORY * history = cmdline->history;
	if (to < 0) {
		to = to + (-to / CMDLINE_HISTORY_SIZE + 1) * CMDLINE_HISTORY_SIZE;
	}
	new_pos = to % CMDLINE_HISTORY_SIZE;
	if (cmdline->history_cursor == new_pos) {
		return false;
	}
	if (history->array[new_pos][0] == '\0' && new_pos != history->index) {
		return false;
	}

	if (cmdline->history_cursor == history->index) {
		strncpy(history->array[history->index], cmdline->string,
				ARRAY_SIZE(history->array[history->index]));
	}

	cmdline->history_cursor = new_pos;
	strcpy(cmdline->string, history->array[new_pos]);
	cmdline->length = strlen(cmdline->string);
	cmdline->cursor = cmdline->length;
	return true;
}

static bool cmdline_history_move_by(CMDLINE *cmdline, int by) {
	return cmdline_history_move_to(cmdline, cmdline->history_cursor + by);
}

bool cmdline_history_backward(CMDLINE *cmdline) {
	return cmdline_history_move_by(cmdline, -1);
}

bool cmdline_history_forward(CMDLINE *cmdline) {
	return cmdline_history_move_by(cmdline, 1);
}

bool cmdline_history_new_entry(CMDLINE *cmdline) {
	bool ret = true;
	CMDLINE_HISTORY * history = cmdline->history;
	if (cmdline->string[0] == '\0') {
		return false;
	}

	if (0 == strcmp(cmdline->string, history->array[(history->index
			+ CMDLINE_HISTORY_SIZE - 1) % CMDLINE_HISTORY_SIZE])) {
		ret = false;
	} else {
		strcpy(history->array[history->index], cmdline->string);

		history->index = (history->index + 1) % CMDLINE_HISTORY_SIZE;
		history->array[history->index][0] = '\0';
		history->array[(history->index + 1) % CMDLINE_HISTORY_SIZE][0] = '\0';
	}

	cmdline->history_cursor = history->index;
	cmdline->length = 0;
	cmdline->cursor = 0;
	cmdline->string[0] = '\0';

	return ret;
}

bool cmdline_cursor_move_to(CMDLINE *cmdline, int to) {
	int old_cursor = cmdline->cursor;
	to = min(cmdline->length, to);
	to = max(0, to);
	cmdline->cursor = to;
	return cmdline->cursor != old_cursor;
}

bool cmdline_cursor_move_by(CMDLINE *cmdline, int by) {
	return cmdline_cursor_move_to(cmdline, cmdline->cursor + by);
}

bool cmdline_cursor_right(CMDLINE *cmdline) {
	return cmdline_cursor_move_by(cmdline, 1);
}

bool cmdline_cursor_left(CMDLINE *cmdline) {
	return cmdline_cursor_move_by(cmdline, -1);
}

bool cmdline_cursor_home(CMDLINE *cmdline) {
	return cmdline_cursor_move_to(cmdline, 0);
}

bool cmdline_cursor_end(CMDLINE *cmdline) {
	return cmdline_cursor_move_to(cmdline, cmdline->length);
}

bool cmdline_chars_delete(CMDLINE *this, int len) {
	int i;
	if (this == NULL) {
		return false;
	}

	len = min(len, this->length - this->cursor);
	len = max(0, len);
	if (len == 0) {
		return false;
	}

	this->length -= len;
	for (i = this->cursor; i < this->length; ++i) {
		this->string[i] = this->string[i + len];
	}
	this->string[this->length] = '\0';

	return true;
}

bool cmdline_chars_backspace(CMDLINE *this, int len) {
	int old_cursor;
	if (this == NULL) {
		return false;
	}

	old_cursor = this->cursor;
	len = max(0, len);

	cmdline_cursor_move_by(this, -len);

	return cmdline_chars_delete(this, old_cursor - this->cursor);
}

bool cmdline_chars_insert(CMDLINE *this, const char *ch, int len) {
	int i;
	if (this == NULL) {
		return false;
	}

	len = min(len, CMDLINE_MAX_LENGTH - this->length);
	len = max(0, len);

	for (i = 0; (i < len) && ch[i]; ++i) {
	}
	len = min(len, i);
	if (len == 0) {
		return false;
	}

	this->length += len;
	this->cursor += len;
	if (!this->is_insert_mode) {
		for (i = this->length; i >= this->cursor; --i) {
			this->string[i] = this->string[i - len];
		}
	}
	for (i = 0; i < len; ++i) {
		this->string[this->cursor - len + i] = ch[i];
	}

	return true;
}

bool cmdline_dc2_reverse(CMDLINE *this) {
	cmdline_chars_insert(this, "\r(reverse-i-search)`':", 22);
	this->cursor -= 2;
	return true;
}

bool cmdline_dc4_reverse(CMDLINE *this) {
	char tmp;
	if (this->cursor != this->length) {
		tmp = this->string[this->cursor];
		this->string[this->cursor] = this->string[this->cursor - 1];
		this->string[this->cursor - 1] = tmp;
		this->cursor++;
	} else {
		tmp = this->string[this->length - 1];
		this->string[this->length - 1] = this->string[this->length - 2];
		this->string[this->length - 2] = tmp;
	}
	return true;
}
