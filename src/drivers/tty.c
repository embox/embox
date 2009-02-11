/*
 * tty.c
 *
 *  Created on: 02.02.2009
 *      Author: Eldar Abusalimov
 */

#include "tty.h"
#include "types.h"
#include "cmdline.h"

// Escape char
#define ESC		'\e'
// ANSI Control Sequence Introducer
#define CSI		'['

// ANSI Control sequence final bytes
#define STD_CODE_CURSOR_UP			'A'
#define STD_CODE_CURSOR_DOWN		'B'
#define STD_CODE_CURSOR_FORWARD		'C'
#define STD_CODE_CURSOR_BACK		'D'
// Select Graphic Rendition
#define STD_CODE_SGR				'm'

#define SGR_COLOR_BLACK		0
#define SGR_COLOR_RED		1

#define PRV_PARAM_HOME		'2'
#define PRV_PARAM_END		'5'

static char _getchar() {
	return uart_getc();
}

static void _putchar(char ch) {
	uart_putc(ch);
}

/*
 * moves screen cursor to specified column
 *
 * param col - column to move screen cursor to
 */
static void remote_move_cursor(int col) {
	static char s[8];
	int i;
	_putchar('\r');
	_putchar(ESC);
	_putchar(CSI);
	sprintf(s, "%d", col);
	for (i = 0; s[i]; i++) {
		_putchar(s[i]);
	}
	_putchar(STD_CODE_CURSOR_FORWARD);
}

/*
 * changes foreground color
 *
 * param color - new color
 */
static void remote_set_foreground_color(int color) {
	if (color < 0 || 9 < color) {
		return;
	}
	_putchar(ESC);
	_putchar(CSI);
	//	Set foreground color code
	_putchar('3');
	_putchar(color + '0');
	_putchar(STD_CODE_SGR);
}

#define PROMPT_MAX_LENGTH		64

static TTY_CALLBACK callback = NULL;

static CMDLINE _cmdline;
static CMDLINE *cmdline;

static char prompt[PROMPT_MAX_LENGTH + 1];
static int prompt_length = 0;

#define REMOTE_CMDLINE_MAX_LENGTH		63

static char remote_cmdline[REMOTE_CMDLINE_MAX_LENGTH + 1];

inline static void move_cmdline_cursor(int col) {
	remote_move_cursor(prompt_length + col);
}

/*
 * moves screen cursor to appropriate place
 */
inline static void flush_cmdline_cursor() {
	move_cmdline_cursor(cmdline->cursor);
}

static void flush_cmdline() {
	int i, cmdline_length;
	BOOL dirty = TRUE;

	move_cmdline_cursor(0);

	for (i = 0; cmdline->string[i]; i++) {
		if (remote_cmdline[i] == '\0') {
			dirty = FALSE;
		}
		if (!dirty || remote_cmdline[i] != cmdline->string[i]) {
			_putchar(cmdline->string[i]);
			remote_cmdline[i] = cmdline->string[i];
		}
	}
	cmdline_length = i;

	// TODO <ESC>[K erases from the current cursor position to the end of the current line
	if (dirty) {
		for (; remote_cmdline[i]; i++) {
			_putchar(' ');
		}
	}

	remote_cmdline[cmdline_length] = '\0';

	flush_cmdline_cursor();
}

static void show_prompt() {
	int i;

	//_putchar('\r');
	//_putchar('\n');
	printf("\n");
	remote_cmdline[0] = '\0';

	cmdline_history_new(cmdline);

	remote_move_cursor(0);
	remote_set_foreground_color(SGR_COLOR_RED);
	// TODO printf
	for (i = 0; prompt[i]; i++) {
		_putchar(prompt[i]);
	}

	remote_set_foreground_color(SGR_COLOR_BLACK);
}

static inline void fire_callback() {
	if (NULL != callback) {
		char buf[cmdline->length + 1];
		sz_cpy(buf, cmdline->string);
		callback(buf);
		show_prompt();
	}
}

inline static void handle_standard_command(const char code, const char *cs) {
	switch (code) {
	case STD_CODE_CURSOR_UP:
		cmdline_history_backward(cmdline);
		flush_cmdline();
		break;
	case STD_CODE_CURSOR_DOWN:
		cmdline_history_forward(cmdline);
		flush_cmdline();
		break;
	case STD_CODE_CURSOR_FORWARD:
		cmdline_cursor_right(cmdline);
		flush_cmdline_cursor();
		break;
	case STD_CODE_CURSOR_BACK:
		cmdline_cursor_left(cmdline);
		flush_cmdline_cursor();
		break;
	default:
		break;
	}
}

inline static void handle_private_command(const char code, const char *cs) {
	int param = cs[1];
	switch (param) {
	case PRV_PARAM_HOME:
		cmdline_cursor_home(cmdline);
		flush_cmdline_cursor();
		break;
	case PRV_PARAM_END:
		cmdline_cursor_end(cmdline);
		flush_cmdline_cursor();
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

typedef struct _CHAR_HANDLER {
	struct _CHAR_HANDLER (* call)(char ch);
} CHAR_HANDLER;

static CHAR_HANDLER regular_char_handler(char ch);
static CHAR_HANDLER escaped_char_handler(char ch);

/*
 * dispatches a regular char
 *
 * param ch
 */
static CHAR_HANDLER regular_char_handler(char ch) {
	static CHAR_HANDLER new_handler;
	new_handler.call = regular_char_handler;

	switch (ch) {
	case ESC:
		new_handler.call = escaped_char_handler;
		break;
	case '\r':
	case '\n':
		fire_callback();
		break;
	case '\b':
		cmdline_char_backspace(cmdline);
		break;
	default:
		// TODO check for char range
		cmdline_char_insert(cmdline, ch);
		flush_cmdline();
		break;
	}

	return new_handler;
}

// control sequence buffer size
#define CS_BUF_SIZE			16

/*
 * dispatches an escaped char
 *
 * param ch
 */
static CHAR_HANDLER escaped_char_handler(char ch) {
	static CHAR_HANDLER new_handler;
	new_handler.call = regular_char_handler;

	static char cs_buf[CS_BUF_SIZE];
	static int p = 0;

	if (p + 1 == CS_BUF_SIZE) {
		// buffer overflow, ignore sequence
		p = 0;
		return new_handler;
	}

	cs_buf[p] = ch;
	cs_buf[p + 1] = 0;

	// check whether first symbol in cs_buf is CSI
	// (ANSI Control Sequence Introducer)
	if (cs_buf[0] != CSI) {
		// bad sequence, ignore it
		p = 0;
		return new_handler;
	}

	// try to detect control sequence
	if (p && detect_control_sequence(cs_buf)) {
		// reset control sequence buffer pointer
		p = 0;
		return new_handler;
	}

	// continue populating control sequence buffer
	p++;
	new_handler.call = escaped_char_handler;
	return new_handler;

}

// in debug purposes
static CHAR_HANDLER debug_char_handler(char ch) {
	static CHAR_HANDLER new_handler;
	new_handler.call = debug_char_handler;

	printf("%c\t%d\t%x\n", ch, ch, ch);
	return new_handler;
}

volatile static BOOL stopped = FALSE;

static void init(TTY_CALLBACK c, const char *w);
static void main_loop();

void tty_start(TTY_CALLBACK c, const char *w) {
	init(c, w);

	stopped = FALSE;
	main_loop();
}

void tty_stop() {
	stopped = FALSE;
}

static CHAR_HANDLER char_handler;

static void main_loop() {
	while (!stopped) {
		char_handler = char_handler.call(_getchar());
	}
}

static void init(TTY_CALLBACK c, const char *w) {
	callback = c;
	prompt_length = sz_cpy(prompt, w);

	cmdline = &_cmdline;
	cmdline_init(cmdline);

	show_prompt();

	char_handler.call = regular_char_handler;
}
