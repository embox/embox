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
#include "kernel/uart.h"
#include "kernel/sys.h"
#include "common.h"
#include "string.h"
#include "drivers/terminal.h"

#define EDIT_MODEL(console, update, action, params...) \
		do if((action)((console)->model, ##params)) { \
			(update)((console)->view, (console)->model); \
		} while(0)

#define CB_EDIT_MODEL(action, params...) \
		EDIT_MODEL((CONSOLE *) cb->outer, screen_out_update, action, ##params)

CONSOLE *cur_console = NULL;

static void on_new_line(SCREEN_CALLBACK *cb, SCREEN *view) {
	CONSOLE *this = (CONSOLE *) cb->outer;
	/*FIXME: resolve "(reverse-i-search)`':" statement */
	char *cmd;// = strstr(this->model->string, ":");
	//        if (cmd)
	//    		*cmd++;
	//    	else
	cmd = this->model->string;

	if (this->callback != NULL && this->callback->exec != NULL && *cmd) {
		screen_out_puts(this->view, NULL);
		char buf[CMDLINE_MAX_LENGTH + 1];
		strcpy(buf, cmd);
		this->callback->exec(this->callback, this, buf);
	}
	screen_out_show_prompt(this->view, this->prompt);
	CB_EDIT_MODEL(cmdline_history_new_entry);
}

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

static void on_home(SCREEN_CALLBACK *cb, SCREEN *view) {
	CB_EDIT_MODEL(cmdline_cursor_home);
}

static void on_end(SCREEN_CALLBACK *cb, SCREEN *view) {
	CB_EDIT_MODEL(cmdline_cursor_end);
}

static void on_insert(SCREEN_CALLBACK *cb, SCREEN *view) {
	CONSOLE *this = (CONSOLE *) cb->outer;
	CMDLINE *cmd = this->model;
	cmd->is_insert_mode = cmd->is_insert_mode ? 0 : 1;
}

static void on_etx(SCREEN_CALLBACK *cb, SCREEN *view) {
	sys_exec_stop();
	on_new_line(cb, view);
}

static void on_eot(SCREEN_CALLBACK *cb, SCREEN *view) {
	console_stop(cur_console);
}

static void on_dc2(SCREEN_CALLBACK *cb, SCREEN *view) {
	CB_EDIT_MODEL(cmdline_dc2_reverse);
}

static void on_dc4(SCREEN_CALLBACK *cb, SCREEN *view) {
	CB_EDIT_MODEL(cmdline_dc4_reverse);
}

#define MAX_PROPOSALS	64

static void on_tab(SCREEN_CALLBACK *cb, SCREEN *view) {
	CONSOLE *this = (CONSOLE *) cb->outer;
	if (this->callback != NULL && this->callback->guess != NULL) {
		char buf[CMDLINE_MAX_LENGTH + 1];
		strncpy(buf, this->model->string, this->model->cursor);
		buf[this->model->cursor] = '\0';

		static char* proposals[MAX_PROPOSALS];
		int proposals_len;
		int offset;
		int common;
		this->callback->guess(this->callback, this, buf, MAX_PROPOSALS,
				&proposals_len, proposals, &offset, &common);
		if (proposals_len == 1) {
			cmdline_chars_insert(this->model, proposals[0] + offset, strlen(
					proposals[0] + offset));
			CB_EDIT_MODEL(cmdline_chars_insert, " ", 1);
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

	cur_console = this;
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
	INIT_MEMBER(screen_callback,on_end);
	INIT_MEMBER(screen_callback,on_insert);
	INIT_MEMBER(screen_callback,on_etx);
	INIT_MEMBER(screen_callback,on_eot);
	INIT_MEMBER(screen_callback,on_dc2);
	INIT_MEMBER(screen_callback,on_dc4);
	screen_callback->outer = this;

	static const char * default_prompt = "";
	strncpy(this->prompt, (prompt != NULL) ? prompt : default_prompt,
			CONFIG_MAX_PROMPT_LENGTH);

	screen_out_show_prompt(this->view, this->prompt);
	screen_in_start(this->view, screen_callback);
}

void console_stop(CONSOLE *this) {
	screen_out_puts(this->view, "\rConsole exit!");
	screen_in_stop(this->view);
}
