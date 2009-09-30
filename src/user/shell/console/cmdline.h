/*
 * cmdline.h
 *
 * Internal representation of command line entity.
 *
 *  Created on: 08.02.2009
 *      Author: Eldar Abusalimov
 */

#ifndef CMDLINE_H_
#define CMDLINE_H_

#include "asm/types.h"

#define CMDLINE_MAX_LENGTH		127
#define CMDLINE_HISTORY_SIZE	8

/*
 * Command line editing history
 */
typedef struct _CMDLINE_HISTORY {

	/* history itself */
	char array[CMDLINE_HISTORY_SIZE][CMDLINE_MAX_LENGTH + 1];

	/* last entry in history */
	int index;

} CMDLINE_HISTORY;

/*
 * Command line internal representation
 */
typedef struct _CMDLINE {

	/* line itself */
	char string[CMDLINE_MAX_LENGTH + 1];

	/* string length */
	int length;

	/* cursor current position */
	int cursor;

	/* editing history */
	CMDLINE_HISTORY history[1];

	/* current position in history */
	int history_cursor;

} CMDLINE;

CMDLINE * cmdline_init(CMDLINE *);

BOOL cmdline_history_backward(CMDLINE *cmdline);

BOOL cmdline_history_forward(CMDLINE *cmdline);

BOOL cmdline_history_new_entry(CMDLINE *cmdline);

BOOL cmdline_cursor_move_to(CMDLINE *cmdline, int to);

BOOL cmdline_cursor_move_by(CMDLINE *cmdline, int by);

BOOL cmdline_cursor_right(CMDLINE *cmdline);

BOOL cmdline_cursor_left(CMDLINE *cmdline);

BOOL cmdline_cursor_home(CMDLINE *cmdline);

BOOL cmdline_cursor_end(CMDLINE *cmdline);

BOOL cmdline_chars_delete(CMDLINE *cmdline, int len);

BOOL cmdline_chars_backspace(CMDLINE *cmdline, int len);

BOOL cmdline_chars_insert(CMDLINE *cmdline, char *ch, int len);

#endif /* CMDLINE_H_ */
