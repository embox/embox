/**
 * @file
 * @brief Console command prompt.
 *
 * @details Supports typing proposals and custom welcome messages.
 *
 * Implementation of MVC with passive model.
 * This entity is controller, cmdline is model, and screen is view.
 *
 * @author Eldar Abusalimov
 */

#include "console.h"
#include <kernel/diag.h>
#include <string.h>
#include <drivers/terminal.h>

#define EDIT_MODEL(console, update, action) \
		do if (action) { \
			(update)((console)->view, (console)->model); \
		} while (0)

#define CB_MODEL(cb) (((CONSOLE *) cb->outer)->model)
#define CB_EDIT_MODEL(action) \
		EDIT_MODEL((CONSOLE *) cb->outer, screen_out_update, action)

CONSOLE *cur_console = NULL;

static int on_new_line(SCREEN_CALLBACK *cb, SCREEN *view, int by) {
	CONSOLE *this = (CONSOLE *) cb->outer;
	char buf[CMDLINE_MAX_LENGTH + 1];
	/*FIXME: resolve "(reverse-i-search)`':" statement */
	char *cmd;

#if 0
	= strstr(this->model->string, ":");
	        if (cmd)
	    		*cmd++;
	    	else
#endif
	cmd = this->model->string;

	if (this->callback != NULL && this->callback->exec != NULL && *cmd) {
		screen_out_puts(this->view, NULL);
		strcpy(buf, cmd);
		this->callback->exec(this->callback, this, buf);
	}
	CB_EDIT_MODEL(cmdline_history_new_entry(CB_MODEL(cb)));
	screen_out_show_prompt(this->view, this->prompt);

	return 0;
}

static int on_char(SCREEN_CALLBACK *cb, SCREEN *view, int ch) {
	char tmp_ch = (char)ch;
	CB_EDIT_MODEL(cmdline_chars_insert((CB_MODEL(cb)), &tmp_ch, 1));
	return 0;
}

static int on_cursor_up(SCREEN_CALLBACK *cb, SCREEN *view, int by) {
	CB_EDIT_MODEL(cmdline_history_backward(CB_MODEL(cb)));
	return 0;
}

static int on_cursor_down(SCREEN_CALLBACK *cb, SCREEN *view, int by) {
	CB_EDIT_MODEL(cmdline_history_forward(CB_MODEL(cb)));
	return 0;
}

static int on_cursor_left(SCREEN_CALLBACK *cb, SCREEN *view, int by) {
	CB_EDIT_MODEL(cmdline_cursor_left(CB_MODEL(cb)));
	return 0;
}

static int on_cursor_right(SCREEN_CALLBACK *cb, SCREEN *view, int by) {
	CB_EDIT_MODEL(cmdline_cursor_right(CB_MODEL(cb)));
	return 0;
}

static int on_backspace(SCREEN_CALLBACK *cb, SCREEN *view, int by) {
	CB_EDIT_MODEL(cmdline_chars_backspace(CB_MODEL(cb), 1));
	return 0;
}

static int on_delete(SCREEN_CALLBACK *cb, SCREEN *view, int by) {
	CB_EDIT_MODEL(cmdline_chars_delete(CB_MODEL(cb), 1));
	return 0;
}

static int on_home(SCREEN_CALLBACK *cb, SCREEN *view, int by) {
	CB_EDIT_MODEL(cmdline_cursor_home(CB_MODEL(cb)));
	return 0;
}

static int on_end(SCREEN_CALLBACK *cb, SCREEN *view, int by) {
	CB_EDIT_MODEL(cmdline_cursor_end(CB_MODEL(cb)));
	return 0;
}

static int on_insert(SCREEN_CALLBACK *cb, SCREEN *view, int by) {
	CONSOLE *this = (CONSOLE *) cb->outer;
	CMDLINE *cmd = this->model;
	cmd->is_insert_mode = cmd->is_insert_mode ? 0 : 1;
	return 0;
}

static int on_etx(SCREEN_CALLBACK *cb, SCREEN *view, int by) {
	on_new_line(cb, view, 0);
	return 0;
}

static int on_eot(SCREEN_CALLBACK *cb, SCREEN *view, int by) {
	console_stop(cur_console);
	return 0;
}

static int on_dc2(SCREEN_CALLBACK *cb, SCREEN *view, int by) {
	CB_EDIT_MODEL(cmdline_dc2_reverse(CB_MODEL(cb)));
	return 0;
}

static int on_dc4(SCREEN_CALLBACK *cb, SCREEN *view, int by) {
	CB_EDIT_MODEL(cmdline_dc4_reverse(CB_MODEL(cb)));
	return 0;
}

static int on_ack(SCREEN_CALLBACK *cb, SCREEN *view, int by) {
	/* Not implemented. */
	return 0;
}

#define MAX_PROPOSALS	64

static int on_tab(SCREEN_CALLBACK *cb, SCREEN *view, int by) {
	CONSOLE *this = (CONSOLE *) cb->outer;
	static const char *proposals[MAX_PROPOSALS];
	int proposals_len, offset, common;
	if (this->callback != NULL && this->callback->guess != NULL) {
		char buf[CMDLINE_MAX_LENGTH + 1];
		strncpy(buf, this->model->string, this->model->cursor);
		buf[this->model->cursor] = '\0';

		this->callback->guess(this->callback, this, buf, MAX_PROPOSALS,
				&proposals_len, proposals, &offset, &common);
		if (proposals_len == 1) {
			cmdline_chars_insert(this->model, proposals[0] + offset, strlen(
					proposals[0] + offset));
			CB_EDIT_MODEL(cmdline_chars_insert(CB_MODEL(cb), " ", 1));
		} else if (proposals_len > 0) {
			if (common > 0) {
				CB_EDIT_MODEL(cmdline_chars_insert(CB_MODEL(cb), proposals[0] + offset, common));
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
	return 0;
}

CONSOLE * console_init(CONSOLE *this, CONSOLE_CALLBACK *callback) {
	//static SCREEN_IO view_io = { diag_getc, diag_putc };
	static SCREEN_IO view_io = { diag_getc, diag_putc };
	if (this == NULL || callback == NULL) {
		return NULL;
	}

	this->callback = callback;

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
	static SCREEN_CALLBACK screen_callback[1];
	static const char * default_prompt = "";
	if (this == NULL) {
		return;
	}
/*TODO may be do static initialize*/
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
	INIT_MEMBER(screen_callback,on_ack);
	screen_callback->outer = this;

	strncpy(this->prompt, (prompt != NULL) ? prompt : default_prompt,
			CONFIG_MAX_PROMPT_LENGTH);

	screen_out_show_prompt(this->view, this->prompt);
	screen_in_start(this->view, screen_callback);
}

void console_stop(CONSOLE *this) {
	screen_out_puts(this->view, "\rConsole exit!");
	screen_in_stop(this->view);
}
