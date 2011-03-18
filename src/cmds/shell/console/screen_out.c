/**
 * @file
 *
 * @date 28.02.09
 * @author Eldar Abusalimov
 */
#include "screen.h"
#include <drivers/terminal.h>
#include <assert.h>
#include <embox/kernel.h>

static void transmit_string(SCREEN *this, const char *str) {
	while (*str) {
		terminal_transmit_va(this->terminal, *str++, 0);
	}
}

/**
 * moves screen cursor by specified value
 * @param by - delta to move screen cursor by
 */
static void transmit_move_cursor_by(SCREEN *this, int by) {
	if (by == 0) {
		return;
	}

	terminal_transmit_va(this->terminal, by > 0 ? TERMINAL_TOKEN_CURSOR_RIGHT
			: TERMINAL_TOKEN_CURSOR_LEFT, 1, by > 0 ? by : -by);
}

static void move_cursor_to(SCREEN *this, int col) {
	/* TODO range checking */

	if (this->cursor + 3 < col || col < this->cursor) {
		/* it is more easy to move cursor by special command*/
		transmit_move_cursor_by(this, col - this->cursor);
		this->cursor = col;
	} else if (this->cursor < col) {
		/* rewrite a few chars*/
		for (; this->cursor < col; ++this->cursor) {
			terminal_transmit_va(this->terminal, this->string[this->cursor], 0);
		}
	}

}

void screen_out_update(SCREEN *this, CMDLINE *cmdline) {
	int i;
	bool dirty = true;

	if (this == NULL) {
		return;
	}

	for (i = 0; cmdline->string[i]; i++) {
		if (this->string[i] == '\0') {
			dirty = false;
		}
		if (!dirty || this->string[i] != cmdline->string[i]) {
			move_cursor_to(this, i);
			assert(i == this->cursor);
			terminal_transmit_va(this->terminal, cmdline->string[i], 0);
			this->cursor++;
			this->string[i] = cmdline->string[i];
		}
	}

	if (dirty && this->string[i] != '\0') {
		move_cursor_to(this, i);
		terminal_transmit_va(this->terminal, TERMINAL_TOKEN_ERASE_SCREEN, 1,
				TERMINAL_TOKEN_PARAM_ERASE_DOWN_RIGHT);
	}
	this->string[i] = '\0';

	move_cursor_to(this, cmdline->cursor);
}

void screen_out_puts(SCREEN *this, const char *str) {
	if (this == NULL) {
		return;
	}
	if (str != NULL) {
		transmit_string(this, str);
	}
	terminal_transmit_va(this->terminal, TERMINAL_TOKEN_CR, 0);
	terminal_transmit_va(this->terminal, TERMINAL_TOKEN_LF, 0);
}

void screen_out_show_prompt(SCREEN *this, const char *prompt) {
	static const char * default_prompt = "";
	if (this == NULL) {
		return;
	}

	this->prompt = (prompt != NULL) ? prompt : default_prompt;

	screen_out_puts(this, NULL);
	*this->string = '\0';
	this->cursor = 0;

#if 0
		screen_set_foreground_color(SGR_COLOR_RED);
		terminal_transmit_va(this->terminal, TERMINAL_TOKEN_CURSOR_SAVE_ATTRS, NULL);
#endif

	terminal_transmit_va(this->terminal, TERMINAL_TOKEN_SGR, 1,
			TERMINAL_TOKEN_PARAM_SGR_FG_RED);
	terminal_transmit_va(this->terminal, TERMINAL_TOKEN_SGR, 1,
			TERMINAL_TOKEN_PARAM_SGR_INTENSITY_BOLD);

	transmit_string(this, prompt);


	terminal_transmit_va(this->terminal, TERMINAL_TOKEN_SGR, 1,
			TERMINAL_TOKEN_PARAM_SGR_RESET);
#if 0
	screen_set_foreground_color(SGR_COLOR_BLACK);
#endif
}
