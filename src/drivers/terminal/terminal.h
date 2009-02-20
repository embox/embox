/*
 * terminal.h
 *
 * ANSI/VT100 terminal driver
 *
 *  Created on: 02.02.2009
 *      Author: Eldar Abusalimov
 */

#ifndef TERMINAL_H_
#define TERMINAL_H_

#include "types.h"
#include "vtparse.h"
#include "vtbuild.h"

typedef enum {
	TERMINAL_COMMAND_BACKSPACE,
	TERMINAL_COMMAND_DELETE,
	TERMINAL_COMMAND_ENTER,
	TERMINAL_COMMAND_CURSOR_UP,
	TERMINAL_COMMAND_CURSOR_DOWN,
	TERMINAL_COMMAND_CURSOR_FORWARD,
	TERMINAL_COMMAND_CURSOR_BACKWARD,
	TERMINAL_COMMAND_CURSOR_SAVE,
	TERMINAL_COMMAND_CURSOR_RESTORE,
} TERMINAL_COMMAND;

/*
 * Terminal callback functions
 */
typedef struct _TERMINAL_CALLBACK {

	void (*on_char)(char ch);

	void (*on_command)(TERMINAL_COMMAND key);

} TERMINAL_CALLBACK;

/*
 * Terminal input/output functions
 */
typedef struct _TERMINAL_IO {

	char (*getc)();

	void (*putc)(char ch);

} TERMINAL_IO;

struct _TERMINAL;

typedef struct _CHAR_HANDLER {

	const struct _CHAR_HANDLER * (* call)(struct _TERMINAL *terminal, char ch);

} CHAR_HANDLER;

/*
 * Terminal internal representation
 */
typedef struct _TERMINAL {

	TERMINAL_CALLBACK callback[1];

	TERMINAL_IO io[1];

	volatile BOOL running;

	VTPARSER parser[1];

	VTBUILDER builder[1];

} TERMINAL;

void terminal_init(TERMINAL *terminal, TERMINAL_CALLBACK *callback,
		TERMINAL_IO *io);

void terminal_send_command(TERMINAL *terminal, TERMINAL_COMMAND command);

void terminal_send_char(TERMINAL *terminal, char ch);

void terminal_start(TERMINAL *terminal);

void terminal_stop(TERMINAL *terminal);

//BOOL terminal_cursor_save(TERMINAL *terminal);
//
//BOOL terminal_cursor_restore(TERMINAL *terminal);
//
//BOOL terminal_get_cursor_position(TERMINAL *terminal, int *x, int *y);
//
//static void remote_set_foreground_color(int color);
//
//static void remote_set_line_wrap(BOOL wrap);
//
//static void remote_erase_screen_down();
//
//static void remote_erase_screen();

#endif /* TERMINAL_H_ */
