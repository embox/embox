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
#include <drivers/iodev.h>

#include "console.h"


#if 1
#define CB_EDIT_MODEL(action) \
	if(action) { \
		screen_out_update(cb->outer->view, cb->outer->model); \
	}
#endif

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
	CB_EDIT_MODEL(cmdline_history_new_entry(cb->outer->model));
	screen_out_show_prompt(this->view, this->prompt);

	return 0;
}

#define MAX_PROPOSALS	64

static int on_tab(SCREEN_CALLBACK *cb, SCREEN *view, int by) {
	CONSOLE *this = cb->outer;
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
			CB_EDIT_MODEL(cmdline_chars_insert(cb->outer->model, " ", 1));
		} else if (proposals_len > 0) {
			if (common > 0) {
				CB_EDIT_MODEL(cmdline_chars_insert(cb->outer->model, proposals[0] + offset, common));
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
	static SCREEN_IO view_io = { iodev_getc, iodev_putc };
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
 * screen
 */
static void handle_char_token(SCREEN *this, TERMINAL_TOKEN ch) {
	char tmp_ch[] = { (char) ch };
	SCREEN_CALLBACK *cb = this->callback;

	if (cb == NULL) {
		return;
	}

	CB_EDIT_MODEL(cmdline_chars_insert(cb->outer->model, tmp_ch, 1));
}

static void handle_ctrl_token(SCREEN *this, TERMINAL_TOKEN token,
		short *params, int params_len) {
	SCREEN_CALLBACK *cb = this->callback;
	static TERMINAL_TOKEN prev_token = TERMINAL_TOKEN_EMPTY;
	if (cb == NULL) {
		return;
	}

	switch (token) {
	case TERMINAL_TOKEN_CURSOR_LEFT:
		CB_EDIT_MODEL(cmdline_cursor_left(cb->outer->model));
		break;
	case TERMINAL_TOKEN_CURSOR_RIGHT:
		CB_EDIT_MODEL(cmdline_cursor_right(cb->outer->model));
		break;
	case TERMINAL_TOKEN_CURSOR_UP:
		CB_EDIT_MODEL(cmdline_history_backward(cb->outer->model));
		break;
	case TERMINAL_TOKEN_CURSOR_DOWN:
		CB_EDIT_MODEL(cmdline_history_forward(cb->outer->model));
		break;
	case TERMINAL_TOKEN_BS:
		CB_EDIT_MODEL(cmdline_chars_backspace(cb->outer->model, 1));
		break;
	case TERMINAL_TOKEN_DEL:
		CB_EDIT_MODEL(cmdline_chars_delete(cb->outer->model, 1));
		break;
	case TERMINAL_TOKEN_END:
		/* TODO: strange char 'F' */
		CB_EDIT_MODEL(cmdline_cursor_end(cb->outer->model));
		break;
	case TERMINAL_TOKEN_ETX:
		on_new_line(cb, this, 0);
		break;
	case TERMINAL_TOKEN_EOT:
		// on_eot(cb, this, 0);
		break;
	case TERMINAL_TOKEN_DC2:
		CB_EDIT_MODEL(cmdline_dc2_reverse(cb->outer->model));
		break;
	case TERMINAL_TOKEN_DC4:
		CB_EDIT_MODEL(cmdline_dc4_reverse(cb->outer->model));
		break;
	case TERMINAL_TOKEN_ACK:
		// on_ack(cb, this, 0);
		break;
	case TERMINAL_TOKEN_LF:
		if (prev_token == TERMINAL_TOKEN_CR) {
			break;
		}
		/* FALLTHROUGH */
	case TERMINAL_TOKEN_CR:
		on_new_line(cb, this, 0);
		break;
	case TERMINAL_TOKEN_PRIVATE:
		if (params_len == 0) {
			break;
		}
		switch (params[0]) {
		case TERMINAL_TOKEN_PARAM_PRIVATE_DELETE:
			CB_EDIT_MODEL(cmdline_chars_delete(cb->outer->model, 1));
			break;
		case TERMINAL_TOKEN_PARAM_PRIVATE_HOME:
			CB_EDIT_MODEL(cmdline_cursor_home(cb->outer->model));
			break;
		case TERMINAL_TOKEN_PARAM_PRIVATE_INSERT:
			cb->outer->model->is_insert_mode = !cb->outer->model->is_insert_mode;
			break;
		default:
			break;
		}
		break;
	case TERMINAL_TOKEN_HT:
		on_tab(cb, this, 0);
		break;
	default:
		break;
	}

	prev_token = token;
}


void screen_in_start(SCREEN *this, SCREEN_CALLBACK *cb) {
	static TERMINAL_TOKEN token;
	short *params;
	int params_len;
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
			handle_char_token(this, token);
		} else {
			handle_ctrl_token(this, token, params, params_len);
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
