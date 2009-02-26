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
#include "vt.h"
#include "vtparse.h"
#include "vtbuild.h"

typedef VT_PARAMS TERMINAL_PARAMS;

typedef enum {
	TERMINAL_TOKEN_TYPE_CTRL, TERMINAL_TOKEN_TYPE_CHAR,
} TERMINAL_TOKEN_TYPE;

#define CTRL(action, code)		(((action) << 8) | (code) & 0xFF)
#define UNCTRL_ACTION(ctrl)		((ctrl) >> 8)
#define UNCTRL_CODE(ctrl)		((ctrl) & 0xFF)

typedef enum {
	TERMINAL_TOKEN_CTRL_CURSOR_UP = CTRL(VT_ACTION_CSI_DISPATCH, VT_CODE_CS_CUU),
	TERMINAL_TOKEN_CTRL_CURSOR_DOWN = CTRL(VT_ACTION_CSI_DISPATCH, VT_CODE_CS_CUD),
	TERMINAL_TOKEN_CTRL_CURSOR_FORWARD = CTRL(VT_ACTION_CSI_DISPATCH, VT_CODE_CS_CUF),
	TERMINAL_TOKEN_CTRL_CURSOR_BACKWARD = CTRL(VT_ACTION_CSI_DISPATCH, VT_CODE_CS_CUB),
	TERMINAL_TOKEN_CTRL_CURSOR_POSITION = CTRL(VT_ACTION_CSI_DISPATCH, VT_CODE_CS_CUP),
	TERMINAL_TOKEN_CTRL_CURSOR_SAVE = CTRL(VT_ACTION_CSI_DISPATCH, VT_CODE_CS_SCP),
	TERMINAL_TOKEN_CTRL_CURSOR_RESTORE = CTRL(VT_ACTION_CSI_DISPATCH, VT_CODE_CS_RCP),

	TERMINAL_TOKEN_CTRL_CURSOR_SAVE_ATTRS = CTRL(VT_ACTION_ESC_DISPATCH, VT_CODE_ESC_SCA),
	TERMINAL_TOKEN_CTRL_CURSOR_RESTORE_ATTRS = CTRL(VT_ACTION_ESC_DISPATCH, VT_CODE_ESC_RCA),

	//	TERMINAL_TOKEN_CTRL_DELETE = 6, // TODO check it
	TERMINAL_TOKEN_CTRL_ENTER = CTRL(VT_ACTION_PRINT, '\n'),
	TERMINAL_TOKEN_CTRL_BACKSPACE = CTRL(VT_ACTION_PRINT, '\b'),
} TERMINAL_TOKEN_CTRL;

typedef char TERMINAL_TOKEN_CHAR;

typedef union {
	TERMINAL_TOKEN_CHAR ch;
	TERMINAL_TOKEN_CTRL ctrl;
} TERMINAL_TOKEN;

/*
 * Terminal input/output functions
 */
typedef struct {

	char (*getc)();

	void (*putc)(char ch);

} TERMINAL_IO;

/*
 * Saves some useful but volatile information
 * about parser state collected at callback invocation time.
 * NOTE: This structure is tightly relies on vtparse algorithms.
 */
typedef struct {
	VT_ACTION action;
	VT_PARAMS params[1];
	char ch;
} VTPARSER_FOOTPRINT;

/* NOTE: This value is tightly relies on vtparse algorithms. */
#define VTPARSER_FOOTPRINTS_AMOUNT 3

/*
 * Terminal internal representation
 */
typedef struct {

	TERMINAL_IO io[1];

	VTBUILDER builder[1];

	VTPARSER parser[1];
	VTPARSER_FOOTPRINT vtparser_footprint[VTPARSER_FOOTPRINTS_AMOUNT];
	int footprint_counter;

} TERMINAL;

void terminal_init(TERMINAL *terminal, TERMINAL_IO *io);

BOOL terminal_receive(TERMINAL *terminal, TERMINAL_TOKEN_TYPE *type,
		TERMINAL_TOKEN *token, TERMINAL_PARAMS *params);

BOOL terminal_transmit(TERMINAL *terminal, TERMINAL_TOKEN_TYPE type,
		TERMINAL_TOKEN *token, TERMINAL_PARAMS *params);

#endif /* TERMINAL_H_ */
