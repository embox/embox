/*
 * cmdline.h
 *
 *  Created on: 08.02.2009
 *      Author: Eldar Abusalimov
 */

#ifndef CMDLINE_H_
#define CMDLINE_H_

#include "types.h"

/*
 * Command line internal representation
 */
typedef struct _CMDLINE {

	/* line itself */
	char * string;

	/* string length */
	unsigned int length;

	/* cursor current position */
	unsigned int cursor;

	/* current position in history */
	unsigned int history_pos;

} CMDLINE;

BOOL cmdline_init(CMDLINE *cmdline);

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
