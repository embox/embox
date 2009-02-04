/*
 * tty.c
 *
 *  Created on: 02.02.2009
 *      Author: Eldar Abusalimov
 */

#include "tty.h"
#include "types.h"
#include "string.h"

#define ESC		'\e'
// ANSI Control Sequence Introducer
#define CSI		'['

// ANSI Control sequence final bytes
#define STD_CODE_CURSOR_UP		'A'
#define STD_CODE_CURSOR_DOWN		'B'
#define STD_CODE_CURSOR_FORWARD	'C'
#define STD_CODE_CURSOR_BACK		'D'
// Cursor Horizontal Absolute
#define STD_CODE_CHA				'G'
// Select Graphic Rendition
#define STD_CODE_SGR				'm'

#define SGR_COLOR_BLACK		0
#define SGR_COLOR_RED		1

#define PRV_PARAM_HOME		'2'
#define PRV_PARAM_END		'5'

// control sequence buffer size
#define CS_BUF_SIZE			16

#define ENTERED_MAX_LENGTH		64
#define ENTERED_HISTORY_SIZE	8

inline static char getc() {
	return uart_getc();
}

inline static void do_putc(char ch) {
	uart_putc(ch);
}

static void putc(char ch) {
	static prev_ch = 0;

	if (ch == '\n') {
		if (prev_ch != '\r') {
			do_putc('\r');
		} else {
			prev_ch = ch;
			return;
		}
	}

	do_putc(ch);

	if (ch == '\r') {
		do_putc('\n');
	}

	prev_ch = ch;
}

/*
 * moves screen cursor to specified column
 *
 * param col - column to move screen cursor to
 */
static void move_line_cursor(int col) {
	static char s[8];
	int i;
	do_putc(ESC);
	do_putc(CSI);
	sprintf(s, "%d", col);
	for (i = 0; s[i]; i++) {
		putc(s[i]);
	}
	do_putc(STD_CODE_CHA);
}

/*
 * changes foreground color
 *
 * param color - new color
 */
static void set_foreground_color(int color) {
	if (color < 0 || 9 < color) {
		return;
	}
	do_putc(ESC);
	do_putc(CSI);
	//	Set foreground color code
	putc('3');
	putc(color + '0');
	do_putc(STD_CODE_SGR);
}

static TTY_CALLBACK callback = NULL;

static char entered_history[ENTERED_HISTORY_SIZE][ENTERED_MAX_LENGTH + 1];
static int entered_history_ptr = 0;

static char *entered = entered_history[0];
static int entered_cursor = 0;
static int entered_length = 0;

static char *welcome = "";

/*
 * moves screen cursor to appropriate place
 */
inline static void update_line_cursor() {
	move_line_cursor(sz_length(welcome) + entered_cursor + 1);
}

static void refresh_entered_line(int start) {
	int i, len;
	int welcome_len;
	if (start < 0) {
		return;
	}
	welcome_len = sz_length(welcome);

	move_line_cursor(welcome_len + start + 1);
	for (i = 0; entered[i]; i++) {
		if (i >= start) {
			putc(entered[i]);
		}
	}
	len = i;
	for (; i < entered_length; i++) {
		if (i >= start) {
			putc(' ');
		}
	}
	entered_length = len;

	move_line_cursor(welcome_len + entered_cursor + 1);
}

static void refresh_full_line() {
	int i;
	int welcome_len = 0;

	move_line_cursor(1);
	set_foreground_color(SGR_COLOR_RED);
	for (i = 0; welcome[i]; i++) {
		putc(welcome[i]);
	}
	set_foreground_color(SGR_COLOR_BLACK);
	welcome_len = i;
	for (i = 0; entered[i]; i++) {
		putc(entered[i]);
	}
	move_line_cursor(welcome_len + entered_cursor + 1);
}

static inline void fire_callback() {
	if (NULL != callback) {
		welcome = callback(entered);
		entered_cursor = 0;
		entered_length = 0;
		entered[0] = 0;
		refresh_full_line();
	}
}

inline static void handle_private_command(const char code, const char *cs) {
	int param = cs[1];
	switch (param) {
	case PRV_PARAM_HOME:
		entered_cursor = 0;
		update_line_cursor();
		break;
	case PRV_PARAM_END:
		entered_cursor = sz_length(entered);
		update_line_cursor();
		break;
	default:
		break;
	}
}

inline static void handle_standard_command(const char code, const char *cs) {
	switch (code) {
	case STD_CODE_CURSOR_UP:
//		if (entered_history_ptr) {
//			entered_history_ptr--;
//		} else {
//			entered_history_ptr = ENTERED_HISTORY_SIZE - 1;
//		}
//		entered = entered_history[entered_history_ptr];
//		entered_cursor = sz_length(entered);
//		refresh_entered_line(0);
		break;
	case STD_CODE_CURSOR_DOWN:
//		if (entered_history_ptr + 1 < ENTERED_HISTORY_SIZE) {
//			entered_history_ptr++;
//		} else {
//			entered_history_ptr = 0;
//		}
//		entered = entered_history[entered_history_ptr];
//		entered_cursor = sz_length(entered);
//		refresh_entered_line(0);
		break;
	case STD_CODE_CURSOR_FORWARD:
		if (entered[entered_cursor] && entered_cursor < ENTERED_MAX_LENGTH) {
			entered_cursor++;
		}
		update_line_cursor();
		break;
	case STD_CODE_CURSOR_BACK:
		if (entered_cursor) {
			entered_cursor--;
		}
		update_line_cursor();
		break;
	default:
		break;
	}
}

/*
 * param cs - null terminated control sequence
 * return TRUE if control sequence has been detected,
 * 		FALSE if not (e.g. it is not completed)
 */
inline static BOOL detect_control_sequence(const char *cs) {
	char code = 0;
	BYTE len = sz_length(cs);
	if (len == 0) {
		return FALSE;
	}
	// ANSI Control sequence final byte
	code = cs[len - 1];

	if (0x40 <= code && code <= 0x6f) {
		handle_standard_command(code, cs);
		return TRUE;
	}

	if (0x70 <= code && code <= 0x7e) {
		handle_private_command(code, cs);
		return TRUE;
	}

	return FALSE;
}

/*
 * dispatches a regular char
 * param ch
 */
inline static void dispatch_char(char ch) {
	switch (ch) {
	case '\r':
	case '\n':
		putc(ch);
		fire_callback();
		break;
	case '\b':
		if (entered_cursor == 0) {
			break;
		}
		int i;
		entered_cursor--;
		for (i = entered_cursor; entered[i]; i++) {
			entered[i] = entered[i + 1];
		}
		refresh_entered_line(entered_cursor);
		break;
	default:
		if (sz_length(entered) == ENTERED_MAX_LENGTH) {
			break;
		}
		if (entered[entered_cursor]) {
			// we are in the middle of the string
			int i;
			char temp;
			for (i = entered_cursor; entered[i]; i++) {
				temp = entered[i];
				entered[i] = ch;
				ch = temp;
			}
			entered[i] = ch;
			entered[i + 1] = 0;
			refresh_entered_line(entered_cursor++);
		} else {
			// we are at the line end
			entered[entered_cursor++] = ch;
			entered[entered_cursor] = 0;
			refresh_entered_line(entered_cursor - 1);
		}
		break;
	}
}

/*
 * dispatches an escaped char
 * param ch
 * return TRUE to leave escape sequence,
 * 		FALSE to continue receiving escaped chars (e.g. sequence is not completed)
 */
inline static BOOL dispatch_escaped_char(char ch) {
	static char cs_buf[CS_BUF_SIZE];
	static int p = 0;

	if (p + 1 == CS_BUF_SIZE) {
		// buffer overflow, ignore sequence
		p = 0;
		return TRUE;
	}

	cs_buf[p] = ch;
	cs_buf[p + 1] = 0;

	// check whether first symbol in cs_buf is CSI
	// (ANSI Control Sequence Introducer)
	if (cs_buf[0] != CSI) {
		// bad sequence, ignore it
		p = 0;
		return TRUE;
	}

	// try to detect control sequence
	if (p && detect_control_sequence(cs_buf)) {
		// reset control sequence buffer pointer
		p = 0;
		return TRUE;
	}

	// continue populating control sequence buffer
	p++;
	return FALSE;

}

static void escaped_char_dispatcher(char ch);

static void regular_char_dispatcher(char ch);

static void (*char_dispatcher)(char ch);

// in debug purposes
static void debug_char_dispatcher(char ch) {
	printf("%c\t%d\t%x\n", ch, ch, ch);
}

void tty_start(TTY_CALLBACK c) {
	callback = c;
	// initial handler fire
	fire_callback();

	char_dispatcher = regular_char_dispatcher;
	while (1) {
		char_dispatcher(getc());
	}

}

static void regular_char_dispatcher(char ch) {
	if (ch == ESC) {
		char_dispatcher = escaped_char_dispatcher;
	} else {
		dispatch_char(ch);
	}
}

static void escaped_char_dispatcher(char ch) {
	if (dispatch_escaped_char(ch)) {
		char_dispatcher = regular_char_dispatcher;
	}
}
