/*
 * cmdline.h
 *
 *  Created on: 08.02.2009
 *      Author: Eldar Abusalimov
 */

#ifndef CMDLINE_H_
#define CMDLINE_H_

#include "types.h"

#define CMDLINE_MAX_LENGTH		63
#define CMDLINE_HISTORY_SIZE	8

/*
 * Command line editing history
 */
typedef struct _CMDLINE_HISTORY {

	/* history itself */
	char array[CMDLINE_HISTORY_SIZE][CMDLINE_MAX_LENGTH + 1];

	/* last entry in history */
	unsigned int index;

} CMDLINE_HISTORY;

/*
 * Command line internal representation
 */
typedef struct _CMDLINE {

	/* line itself */
	char string[CMDLINE_MAX_LENGTH + 1];

	/* string length */
	unsigned int length;

	/* cursor current position */
	unsigned int cursor;

	/* editing history */
	CMDLINE_HISTORY history;

	/* current position in history */
	unsigned int history_cursor;

} CMDLINE;

void cmdline_init(CMDLINE *cmdline);

BOOL cmdline_history_move_to(CMDLINE *cmdline, int to);

BOOL cmdline_history_move_by(CMDLINE *cmdline, int by);

BOOL cmdline_history_backward(CMDLINE *cmdline);

BOOL cmdline_history_forward(CMDLINE *cmdline);

BOOL cmdline_history_new(CMDLINE *cmdline);

BOOL cmdline_cursor_move_to(CMDLINE *cmdline, unsigned int to);

BOOL cmdline_cursor_move_by(CMDLINE *cmdline, int by);

BOOL cmdline_cursor_right(CMDLINE *cmdline);

BOOL cmdline_cursor_left(CMDLINE *cmdline);

BOOL cmdline_cursor_home(CMDLINE *cmdline);

BOOL cmdline_cursor_end(CMDLINE *cmdline);

BOOL cmdline_char_delete(CMDLINE *cmdline);

BOOL cmdline_char_backspace(CMDLINE *cmdline);

BOOL cmdline_char_insert(CMDLINE *cmdline, char ch);

#endif /* CMDLINE_H_ */
