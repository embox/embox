/*
 * Console command prompt.
 *
 * Supports typing proposals and custom welcome messages.
 *
 * Implementation of MVC with passive model.
 * This entity is controller, cmdline is model, and screen is view.
 *
 * Author: Eldar Abusalimov
 */

#include "console.h"
#include "cmdline.h"
#include "kernel/uart.h"
#include "conio.h"
#include "common.h"
#include "string.h"
#include "drivers/terminal.h"

#define EDIT_MODEL(console,update, action, params...)	(action((console)->model, ##params) ? update((console)->view, (console)->model): FALSE)
#define CB_EDIT_MODEL(action, params...)	EDIT_MODEL((CONSOLE *) cb->outer, screen_out_update, action, ##params)

static void on_char(SCREEN_CALLBACK *cb, SCREEN *view, char ch) {
	CB_EDIT_MODEL(cmdline_chars_insert, &ch,1);
}

static void on_cursor_up(SCREEN_CALLBACK *cb, SCREEN *view, int by) {
	CB_EDIT_MODEL(cmdline_history_backward);
}

static void on_cursor_down(SCREEN_CALLBACK *cb, SCREEN *view, int by) {
	CB_EDIT_MODEL(cmdline_history_forward);
}

static void on_cursor_left(SCREEN_CALLBACK *cb, SCREEN *view, int by) {
	CB_EDIT_MODEL(cmdline_cursor_left);
}

static void on_cursor_right(SCREEN_CALLBACK *cb, SCREEN *view, int by) {
	CB_EDIT_MODEL(cmdline_cursor_right);
}

static void on_backspace(SCREEN_CALLBACK *cb, SCREEN *view) {
	CB_EDIT_MODEL(cmdline_chars_backspace,1);
}

static void on_delete(SCREEN_CALLBACK *cb, SCREEN *view) {
	CB_EDIT_MODEL(cmdline_chars_delete,1);
}

#define MAX_PROPOSALS	64

static void on_tab(SCREEN_CALLBACK *cb, SCREEN *view) {
	CONSOLE *this = (CONSOLE *) cb->outer;
	if (this->callback != NULL && this->callback->guess != NULL) {
		char buf[this->model->cursor + 1];
		strncpy(buf, this->model->string, this->model->cursor);
		buf[this->model->cursor] = '\0';

		static char* proposals[MAX_PROPOSALS];
		int proposals_len;
		int offset;
		int common;
		this->callback->guess(this->callback, this, buf, MAX_PROPOSALS,
				&proposals_len, proposals, &offset, &common);
		if (proposals_len == 1) {
			static char space = ' ';
			cmdline_chars_insert(this->model, proposals[0] + offset, strlen(
					proposals[0] + offset));
			CB_EDIT_MODEL(cmdline_chars_insert, &space, 1);
		} else if (proposals_len > 0) {
			if (common > 0) {
				CB_EDIT_MODEL(cmdline_chars_insert, proposals[0] + offset, common);
			} else {
				int i;
				screen_out_puts(this->view, NULL);
				for (i = 0; i < proposals_len; ++i) {
					screen_out_puts(this->view, proposals[i]);
				}
				screen_out_show_prompt(this->view, this->prompt);
				screen_out_update(this->view, this->model);
			}
		}
	}
}

static void on_new_line(SCREEN_CALLBACK *cb, SCREEN *view) {
	CONSOLE *this = (CONSOLE *) cb->outer;
	if (this->callback != NULL && this->callback->exec != NULL
			&& *this->model->string) {
		screen_out_puts(this->view, NULL);
		char buf[CMDLINE_MAX_LENGTH + 1];
		strcpy(buf, this->model->string);
		this->callback->exec(this->callback, this, buf);
	}
	screen_out_show_prompt(this->view, this->prompt);
	CB_EDIT_MODEL(cmdline_history_new_entry);
}

static void on_home(SCREEN_CALLBACK *cb, SCREEN *view) {
	CONSOLE *this = (CONSOLE *) cb->outer;
	//TODO: move cursor to the begin
}

CONSOLE * console_init(CONSOLE *this, CONSOLE_CALLBACK *callback) {
	if (this == NULL || callback == NULL) {
		return NULL;
	}

	this->callback = callback;

	static SCREEN_IO view_io = { uart_getc, uart_putc };

	if (cmdline_init(this->model) == NULL) {
		return NULL;
	}
	if (screen_init(this->view, &view_io) == NULL) {
		return NULL;
	}

	return this;
}

#define INIT_MEMBER(obj, member) 	obj->member = member

void console_start(CONSOLE *this, const char *prompt) {
	if (this == NULL) {
		return;
	}

	static SCREEN_CALLBACK screen_callback[1];
	INIT_MEMBER(screen_callback,on_char);
	INIT_MEMBER(screen_callback,on_cursor_up);
	INIT_MEMBER(screen_callback,on_cursor_left);
	INIT_MEMBER(screen_callback,on_cursor_down);
	INIT_MEMBER(screen_callback,on_cursor_right);
	INIT_MEMBER(screen_callback,on_new_line);
	INIT_MEMBER(screen_callback,on_backspace);
	INIT_MEMBER(screen_callback,on_tab);
	INIT_MEMBER(screen_callback,on_delete);
	INIT_MEMBER(screen_callback,on_home);
	screen_callback->outer = this;

	static const char * default_prompt = "";
	strncpy(this->prompt, (prompt != NULL) ? prompt : default_prompt,
			MONITOR_MAX_PROMPT_LENGTH);

	screen_out_show_prompt(this->view, this->prompt);

	screen_in_start(this->view, screen_callback);
}

void console_stop(CONSOLE *this) {
	screen_in_stop(this->view);
}

