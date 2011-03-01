/**
 * @file
 * @brief Internal representation of command line entity.
 *
 * @date 08.02.2009
 * @author Eldar Abusalimov
 */
#ifndef CMDLINE_H_
#define CMDLINE_H_

#include <types.h>

#define CMDLINE_MAX_LENGTH      127
#define CMDLINE_HISTORY_SIZE    8

/**
 * Command line editing history
 */
typedef struct _CMDLINE_HISTORY {
	char array[CMDLINE_HISTORY_SIZE][CMDLINE_MAX_LENGTH + 1]; /**< history itself */
	int index;                                                /**< last entry in history */
} CMDLINE_HISTORY;

/**
 * Command line internal representation
 */
typedef struct _CMDLINE {
	char string[CMDLINE_MAX_LENGTH + 1]; /**< line itself */
	int length;                          /**< string length */
	int cursor;                          /**< cursor current position */
	CMDLINE_HISTORY history[1];          /**< editing history */
	int history_cursor;                  /**< current position in history */
	char is_insert_mode;                 /**< INSERT mode */
} CMDLINE;

CMDLINE * cmdline_init(CMDLINE *);

bool cmdline_history_backward(CMDLINE *cmdline);
bool cmdline_history_forward(CMDLINE *cmdline);
bool cmdline_history_new_entry(CMDLINE *cmdline);
bool cmdline_cursor_move_to(CMDLINE *cmdline, int to);
bool cmdline_cursor_move_by(CMDLINE *cmdline, int by);
bool cmdline_cursor_right(CMDLINE *cmdline);
bool cmdline_cursor_left(CMDLINE *cmdline);
bool cmdline_cursor_home(CMDLINE *cmdline);
bool cmdline_cursor_end(CMDLINE *cmdline);
bool cmdline_chars_delete(CMDLINE *cmdline, int len);
bool cmdline_chars_backspace(CMDLINE *cmdline, int len);
bool cmdline_chars_insert(CMDLINE *cmdline, const char *ch, int len);
bool cmdline_dc2_reverse(CMDLINE *cmdline);
bool cmdline_dc4_reverse(CMDLINE *cmdline);

#endif /* CMDLINE_H_ */
