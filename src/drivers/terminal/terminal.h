/*
 * Terminal driver.
 *
 * Allows us to operate high-level tokens instead of simple chars.
 * It also provides some common ANSI/VT100 terminal control sequences.
 *
 * This entity is backed by VTParse and VTBuild modules.
 *
 * Author: Eldar Abusalimov
 */

#ifndef TERMINAL_H_
#define TERMINAL_H_

#include "types.h"
#include "vt.h"
#include "vtparse.h"
#include "vtbuild.h"

typedef int TERMINAL_TOKEN;

typedef struct {
	int data[VT_TOKEN_MAX_PARAMS];
	int length;
} TERMINAL_TOKEN_PARAMS;

#define ENCODE(action, char1, char2, code) 	\
	(((action & 0xFF) ^ VT_ACTION_PRINT) << 24 \
		| ((char1 & 0xFF) << 16) \
		| ((char2 & 0xFF) << 8) \
		| (code & 0xFF))

#define DECODE_ACTION(token) (((token >> 24) & 0xFF) ^ VT_ACTION_PRINT)
#define DECODE_CHAR1(token) ((token >> 16) & 0xFF)
#define DECODE_CHAR2(token) ((token >> 8) & 0xFF)
#define DECODE_CODE(token) ((token) & 0xFF)

#define ENCODE_CS_(char1, char2, code)	ENCODE(VT_ACTION_CS_DISPATCH, char1, char2, code)
#define ENCODE_ESC_(char1, char2, code)	ENCODE(VT_ACTION_ESC_DISPATCH, char1, char2, code)

#define ENCODE_CS(code)		ENCODE_CS_(0, 0, code)
#define ENCODE_ESC(code)		ENCODE_ESC_(0, 0, code)
#define ENCODE_EXEC(code)	ENCODE(VT_ACTION_EXECUTE, 0, 0, code)

#define TERMINAL_TOKEN_EMPTY ENCODE(0,0,0,0)

/* Cursor Up */
#define TERMINAL_TOKEN_CURSOR_UP  ENCODE_CS('A')
/* Cursor Down */
#define TERMINAL_TOKEN_CURSOR_DOWN  ENCODE_CS('B')
/* Cursor Forward */
#define TERMINAL_TOKEN_CURSOR_RIGHT  ENCODE_CS('C')
/* Cursor Backward */
#define TERMINAL_TOKEN_CURSOR_LEFT  ENCODE_CS('D')
/* Cursor Position */
#define TERMINAL_TOKEN_CURSOR_POSITION  ENCODE_CS('H')
/* Saves the cursor position. */
#define TERMINAL_TOKEN_CURSOR_SAVE  ENCODE_CS('s')
/* Restores the cursor position. */
#define TERMINAL_TOKEN_CURSOR_RESTORE  ENCODE_CS('u')
/* Saves the cursor position & attributes. */
#define TERMINAL_TOKEN_CURSOR_SAVE_ATTRS  ENCODE_ESC('7')
/* Restores the cursor position & attributes. */
#define TERMINAL_TOKEN_CURSOR_RESTORE_ATTRS  ENCODE_ESC('8')
/*
 * Clear part of the screen.
 * If param is 0 (or missing)), clear from cursor to end of screen.
 * If param is 1), clear from cursor to beginning of the screen.
 * If param is 2), clear entire screen
 */
#define TERMINAL_TOKEN_ERASE_SCREEN  ENCODE_CS('J')
/*
 * Erases part of the line.
 * If param is zero (or missing)), clear from cursor to the end of the line.
 * If param is one), clear from cursor to beginning of the line.
 * If param is two), clear entire line.
 * Cursor position does not change
 */
#define TERMINAL_TOKEN_ERASE_LINE  ENCODE_CS('K')
/* Carriage Return */
#define TERMINAL_TOKEN_CR  ENCODE_EXEC('\r')
/* Line Feed */
#define TERMINAL_TOKEN_LF  ENCODE_EXEC('\n')
/* Horizontal Tabulation */
#define TERMINAL_TOKEN_HT  ENCODE_EXEC('\t')
/* Backspace */
#define TERMINAL_TOKEN_BS  ENCODE_EXEC('\b')
/* Delete */
#define TERMINAL_TOKEN_DEL  ENCODE_EXEC(0x7f)
/* Private */
#define TERMINAL_TOKEN_PRIVATE  ENCODE_CS('~')
/* Set Mode */
#define TERMINAL_TOKEN_SET_MODE  ENCODE_CS('h')
/* Reset Mode */
#define TERMINAL_TOKEN_RESET_MODE  ENCODE_CS('l')
/* Select Graphic Rendition */
#define TERMINAL_TOKEN_SGR  ENCODE_CS('m')

#define TERMINAL_TOKEN_PARAM_MODE_LINE_WRAP 7

#define TERMINAL_TOKEN_PARAM_ERASE_DOWN_RIGHT 	0
#define TERMINAL_TOKEN_PARAM_ERASE_UP_LEFT 	0
#define TERMINAL_TOKEN_PARAM_ERASE_ENTIRE 	2

#define TERMINAL_TOKEN_PARAM_PRIVATE_INSERT	1
#define TERMINAL_TOKEN_PARAM_PRIVATE_HOME	2
#define TERMINAL_TOKEN_PARAM_PRIVATE_DELETE	4
#define TERMINAL_TOKEN_PARAM_PRIVATE_END	5

#define TERMINAL_TOKEN_PARAM_SGR_RESET		0
#define TERMINAL_TOKEN_PARAM_SGR_INTENSITY_BOLD		1
#define TERMINAL_TOKEN_PARAM_SGR_INTENSITY_NORMAL	22
#define TERMINAL_TOKEN_PARAM_SGR_BLINK_SLOW		5
#define TERMINAL_TOKEN_PARAM_SGR_BLINK_OFF	25

#define TERMINAL_TOKEN_PARAM_SGR_FG_BLACK	30
#define TERMINAL_TOKEN_PARAM_SGR_FG_RED		31
#define TERMINAL_TOKEN_PARAM_SGR_FG_GREEN	32
#define TERMINAL_TOKEN_PARAM_SGR_FG_YELLOW	33
#define TERMINAL_TOKEN_PARAM_SGR_FG_BLUE	34
#define TERMINAL_TOKEN_PARAM_SGR_FG_MAGENTA	35
#define TERMINAL_TOKEN_PARAM_SGR_FG_CYAN	36
#define TERMINAL_TOKEN_PARAM_SGR_FG_WHITE	37
#define TERMINAL_TOKEN_PARAM_SGR_FG_RESET	39

#define TERMINAL_TOKEN_PARAM_SGR_BG_BLACK	40
#define TERMINAL_TOKEN_PARAM_SGR_BG_RED		41
#define TERMINAL_TOKEN_PARAM_SGR_BG_GREEN	42
#define TERMINAL_TOKEN_PARAM_SGR_BG_YELLOW	43
#define TERMINAL_TOKEN_PARAM_SGR_BG_BLUE	44
#define TERMINAL_TOKEN_PARAM_SGR_BG_MAGENTA	45
#define TERMINAL_TOKEN_PARAM_SGR_BG_CYAN	46
#define TERMINAL_TOKEN_PARAM_SGR_BG_WHITE	47

/*
 * Terminal input/output functions
 */
typedef struct {

	char (*getc)();

	void (*putc)(char ch);

} TERMINAL_IO;

/*
 * Terminal internal representation
 */
typedef struct {

	TERMINAL_IO io[1];

	VTBUILDER builder[1];

	VTPARSER parser[1];

	/* NOTE: This value is tightly relies on vtparse algorithms. */
#define VTPARSER_TOKEN_QUEUE_AMOUNT 3
	/*
	 * Saves some useful but volatile information
	 * about parser state collected at callback invocation time.
	 * NOTE: This structure is tightly relies on vtparse algorithms.
	 */
	VT_TOKEN vt_token_queue[VTPARSER_TOKEN_QUEUE_AMOUNT];

	int vt_token_queue_len;
	int vt_token_queue_head;

	TERMINAL_TOKEN_PARAMS default_params[1];

} TERMINAL;

TERMINAL * terminal_init(TERMINAL *terminal, TERMINAL_IO *io);

BOOL terminal_receive(TERMINAL *terminal, TERMINAL_TOKEN *token,
		TERMINAL_TOKEN_PARAMS *params);

BOOL terminal_transmit(TERMINAL *terminal, TERMINAL_TOKEN token, int params_len,
		...);

#endif /* TERMINAL_H_ */
