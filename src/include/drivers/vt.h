/**
 * vt.h
 *
 * @date 04.02.2009
 * @author Eldar Abusalimov
 */
#ifndef VT_H_
#define VT_H_

#define VT_TOKEN_MAX_CHARS     2
#define VT_TOKEN_MAX_PARAMS    8

typedef enum {
	VT_ACTION_NONE         = 0,
	VT_ACTION_CLEAR        = 1,
	VT_ACTION_COLLECT      = 2,
	VT_ACTION_CSI_DISPATCH  = 3,
	VT_ACTION_ESC_DISPATCH = 4,
	VT_ACTION_EXECUTE      = 5,
	VT_ACTION_HOOK         = 6,
	VT_ACTION_IGNORE       = 7,
	VT_ACTION_OSC_END      = 8,
	VT_ACTION_OSC_PUT      = 9,
	VT_ACTION_OSC_START    = 10,
	VT_ACTION_PARAM        = 11,
	VT_ACTION_PRINT        = 12,
	VT_ACTION_PUT          = 13,
	VT_ACTION_UNHOOK       = 14
} VT_ACTION;

typedef struct {
	VT_ACTION action;
	char      attrs[VT_TOKEN_MAX_CHARS];
	int       attrs_len;
	const int *params;
	int       params_len;
	char      ch;
} VT_TOKEN;

/** ANSI Escape */
#define ESC		'\033'

/** ANSI Control Sequence Introducer */
#define CSI		'['

#endif /* VT_H_ */
