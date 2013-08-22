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

#include <string.h>
#include <assert.h>

#include <drivers/terminal.h>
#include <drivers/diag.h>

#include "console.h"

#define MAX_PROPOSALS	64

static int on_new_line(SCREEN_CALLBACK *cb, SCREEN *view, int by) {
	CONSOLE *this = cb->outer;
	char buf[CMDLINE_MAX_LENGTH + 1];
	/*FIXME: resolve "(reverse-i-search)`':" statement */
	char *cmd;

	cmd = this->model->string;

	if (this->callback != NULL && this->callback->exec != NULL && *cmd) {
		screen_out_puts(this->view, NULL);
		strcpy(buf, cmd);
		this->callback->exec(this->callback, this, buf);
	}

	screen_out_show_prompt(this->view, this->prompt);

	return cmdline_history_new_entry(cb->outer->model);
}

static int on_tab(SCREEN_CALLBACK *cb, SCREEN *view, int by) {
	CONSOLE *this = cb->outer;
	const char *proposals[MAX_PROPOSALS];

	int proposals_len, offset, common;

	if (this->callback != NULL && this->callback->guess != NULL) {
		char buf[CMDLINE_MAX_LENGTH + 1];
		strncpy(buf, this->model->string, this->model->cursor);
		buf[this->model->cursor] = '\0';

		this->callback->guess(this->callback, this, buf, MAX_PROPOSALS,
				&proposals_len, proposals, &offset, &common);

		if (!proposals_len) {
			return 0;
		}
		if (proposals_len == 1) {
			cmdline_chars_insert(this->model, proposals[0] + offset,
					strlen(proposals[0] + offset));
			cmdline_chars_insert(cb->outer->model, " ", 1);
			return 1;
		}

		if (common > 0) {
			return cmdline_chars_insert(cb->outer->model,
					proposals[0] + offset, common);
		}

		screen_out_puts(this->view, NULL);
		for (int i = 0; i < proposals_len; ++i) {
			screen_out_puts(this->view, proposals[i]);
		}
		screen_out_show_prompt(this->view, this->prompt);
		screen_out_update(this->view, this->model);
	}

	return 0;
}

CONSOLE * console_init(CONSOLE *this, CONSOLE_CALLBACK *callback) {
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

	return this;
}


void console_start(CONSOLE *this, const char *prompt) {
	static SCREEN_CALLBACK screen_callback[1];
	static const char * default_prompt = "";
	if (this == NULL) {
		return;
	}
	screen_callback->outer = this;

	strncpy(this->prompt, (prompt != NULL) ? prompt : default_prompt,
			OPTION_GET(NUMBER, prompt_len));

	screen_out_show_prompt(this->view, this->prompt);
	screen_in_start(this->view, screen_callback);
}

void console_stop(CONSOLE *this) {
	screen_out_puts(this->view, "\rConsole exit!");
	screen_in_stop(this->view);
}

/*
 * Token handling functions modify the cmdline and tell whether the screen
 * should be redrawn or not.
 */

static int handle_char_token(SCREEN *this, TERMINAL_TOKEN ch) {
	char tmp_ch[] = { (char) ch };
	SCREEN_CALLBACK *cb = this->callback;

	assert(cb);

	return cmdline_chars_insert(cb->outer->model, tmp_ch, 1);
}

static int handle_ctrl_token(SCREEN *this, TERMINAL_TOKEN token,
		short *params, int params_len) {
	SCREEN_CALLBACK *cb = this->callback;
	CMDLINE *cmdline;

	assert(cb);

	cmdline = cb->outer->model;

	switch (token) {
	case TERMINAL_TOKEN_CURSOR_LEFT:
		return cmdline_cursor_left(cmdline);
	case TERMINAL_TOKEN_CURSOR_RIGHT:
		return cmdline_cursor_right(cmdline);
	case TERMINAL_TOKEN_CURSOR_UP:
		return cmdline_history_backward(cmdline);
	case TERMINAL_TOKEN_CURSOR_DOWN:
		return cmdline_history_forward(cmdline);
	case TERMINAL_TOKEN_BS:
		return cmdline_chars_backspace(cmdline, 1);
	case TERMINAL_TOKEN_DEL:
		return cmdline_chars_delete(cmdline, 1);
	case TERMINAL_TOKEN_END: /* TODO: strange char 'F' */
		return cmdline_cursor_end(cmdline);
	case TERMINAL_TOKEN_ETX:
		return on_new_line(cb, this, 0);
	case TERMINAL_TOKEN_EOT: // on_eot(cb, this, 0);
		return 0;
	case TERMINAL_TOKEN_DC2:
		return cmdline_dc2_reverse(cmdline);
	case TERMINAL_TOKEN_DC4:
		return cmdline_dc4_reverse(cmdline);
	case TERMINAL_TOKEN_ACK: // on_ack(cb, this, 0);
		return 0;
	case TERMINAL_TOKEN_LF:
	case TERMINAL_TOKEN_CR:
		return on_new_line(cb, this, 0);
	case TERMINAL_TOKEN_PRIVATE:
		if (params_len == 0) {
			return 0;
		}
		switch (params[0]) {
		case TERMINAL_TOKEN_PARAM_PRIVATE_DELETE:
			return cmdline_chars_delete(cmdline, 1);
		case TERMINAL_TOKEN_PARAM_PRIVATE_HOME:
			return cmdline_cursor_home(cmdline);
		case TERMINAL_TOKEN_PARAM_PRIVATE_INSERT:
			cmdline->is_insert_mode = !cmdline->is_insert_mode;
			return 0;
		default:
			return 0;
		}
	case TERMINAL_TOKEN_HT:
		return on_tab(cb, this, 0);
	default:
		return 0;
	}
}


void screen_in_start(SCREEN *this, SCREEN_CALLBACK *cb) {
	static TERMINAL_TOKEN token;
	short *params;
	int params_len;
	int redraw;
	char ch;

	if ((this == NULL) || this->running) {
		return;
	}
	this->running = true;

	this->callback = cb;

	while (this->callback != NULL && terminal_receive(this->terminal, &token,
			&params, &params_len)) {
		ch = token & 0xFF;

		if (ch == token) {
			redraw = handle_char_token(this, token);
		} else {
			redraw = handle_ctrl_token(this, token, params, params_len);
		}

		if (redraw) {
			screen_out_update(cb->outer->view, cb->outer->model);
		}
	}

	assert(this->callback == NULL);
	this->running = false;
}

void screen_in_stop(SCREEN *this) {
	if (this == NULL) {
		return;
	}

	this->callback = NULL;
}
