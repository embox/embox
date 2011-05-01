/**
 * @file
 * @brief Terminal driver.
 * @details
 * Allows us to operate high-level tokens instead of simple chars.
 * It also provides some common ANSI/VT100 terminal control sequences.
 *
 * This entity is backed by VTParse and VTBuild modules.
 *
 * @date 04.02.2009
 * @author Eldar Abusalimov
 */
#ifndef TERMINAL_H_
#define TERMINAL_H_

#include <types.h>
#include <drivers/vtparse.h>
#include <drivers/vtbuild.h>

typedef int TERMINAL_TOKEN;

#define ENCODE(action, char1, char2, code) 	\
	(((action & 0xFF) ^ VT_ACTION_PRINT) << 24 \
		| ((char1 & 0xFF) << 16) \
		| ((char2 & 0xFF) << 8) \
		| (code & 0xFF))

#define DECODE_ACTION(token)                 (((token >> 24) & 0xFF) ^ VT_ACTION_PRINT)
#define DECODE_CHAR1(token)                  ((token >> 16) & 0xFF)
#define DECODE_CHAR2(token)                  ((token >> 8) & 0xFF)
#define DECODE_CODE(token)                   ((token) & 0xFF)

#define ENCODE_CS_(char1, char2, code)	     ENCODE(VT_ACTION_CSI_DISPATCH, char1, char2, code)
#define ENCODE_ESC_(char1, char2, code)	     ENCODE(VT_ACTION_ESC_DISPATCH, char1, char2, code)

#define ENCODE_CS(code)                      ENCODE_CS_(0, 0, code)
#define ENCODE_ESC(code)                     ENCODE_ESC_(0, 0, code)
#define ENCODE_EXEC(code)                    ENCODE(VT_ACTION_EXECUTE, 0, 0, code)

#define TERMINAL_TOKEN_EMPTY                 ENCODE(0,0,0,0)

/* Cursor Up */
#define TERMINAL_TOKEN_CURSOR_UP             ENCODE_CS('A')
/* Cursor Down */
#define TERMINAL_TOKEN_CURSOR_DOWN           ENCODE_CS('B')
/* Cursor Forward */
#define TERMINAL_TOKEN_CURSOR_RIGHT          ENCODE_CS('C')
/* Cursor Backward */
#define TERMINAL_TOKEN_CURSOR_LEFT           ENCODE_CS('D')
/* Cursor Position */
#define TERMINAL_TOKEN_CURSOR_POSITION       ENCODE_CS('H')
/* Saves the cursor position. */
#define TERMINAL_TOKEN_CURSOR_SAVE           ENCODE_CS('s')
/* Restores the cursor position. */
#define TERMINAL_TOKEN_CURSOR_RESTORE        ENCODE_CS('u')
/* Saves the cursor position & attributes. */
#define TERMINAL_TOKEN_CURSOR_SAVE_ATTRS     ENCODE_ESC('7')
/* Restores the cursor position & attributes. */
#define TERMINAL_TOKEN_CURSOR_RESTORE_ATTRS  ENCODE_ESC('8')
/*
 * Clear part of the screen.
 * If param is 0 (or missing)), clear from cursor to end of screen.
 * If param is 1), clear from cursor to beginning of the screen.
 * If param is 2), clear entire screen
 */
#define TERMINAL_TOKEN_ERASE_SCREEN          ENCODE_CS('J')
/*
 * Erases part of the line.
 * If param is zero (or missing)), clear from cursor to the end of the line.
 * If param is one), clear from cursor to beginning of the line.
 * If param is two), clear entire line.
 * Cursor position does not change
 */
#define TERMINAL_TOKEN_ERASE_LINE            ENCODE_CS('K')

/** ASCII table **/

/* Null */
#define TERMINAL_TOKEN_NUL      ENCODE_EXEC(0x00) /* [Ctrl] @ (150994944) */
/* Start of Header */
#define TERMINAL_TOKEN_SOX      ENCODE_EXEC(0x01) /* [Ctrl] A */
/* Start of Text */
#define TERMINAL_TOKEN_STX      ENCODE_EXEC(0x02) /* [Ctrl] B */
/* End of text */
#define TERMINAL_TOKEN_ETX      ENCODE_EXEC(0x03) /* [Ctrl] C */
/* End of Transmission */
#define TERMINAL_TOKEN_EOT      ENCODE_EXEC(0x04) /* [Ctrl] D */
/* Enquiry */
#define TERMINAL_TOKEN_ENQ      ENCODE_EXEC(0x05) /* [Ctrl] E */
/* Acknowledge */
#define TERMINAL_TOKEN_ACK      ENCODE_EXEC(0x06) /* [Ctrl] F */
/* Bell */
#define TERMINAL_TOKEN_BEL      ENCODE_EXEC(0x07) /* [Ctrl] G */
/* Backspace */
#define TERMINAL_TOKEN_BS       ENCODE_EXEC(0x08) /* [Ctrl] H '\b' */
/* Horizontal Tabulation */
#define TERMINAL_TOKEN_HT       ENCODE_EXEC(0x09) /* [Ctrl] I '\t' */
/* Line Feed */
#define TERMINAL_TOKEN_LF       ENCODE_EXEC(0x0A) /* [Ctrl] J '\n' */
/* Vertical tabulation */
#define TERMINAL_TOKEN_VT       ENCODE_EXEC(0x0B) /* [Ctrl] K */
/* Form Feed */
#define TERMINAL_TOKEN_FF       ENCODE_EXEC(0x0C) /* [Ctrl] L */
/* Carriage Return */
#define TERMINAL_TOKEN_CR       ENCODE_EXEC(0x0D) /* [Ctrl] M '\r' */
/* Shift out */
#define TERMINAL_TOKEN_SO       ENCODE_EXEC(0x0E) /* [Ctrl] N */
/* Shift in */
#define TERMINAL_TOKEN_SI       ENCODE_EXEC(0x0F) /* [Ctrl] O */
/* Data Link Escape */
#define TERMINAL_TOKEN_DLE      ENCODE_EXEC(0x10) /* [Ctrl] P */
/* Device Control 1 (XON) */
#define TERMINAL_TOKEN_DC1      ENCODE_EXEC(0x11) /* [Ctrl] Q */
/* Device Control 2 */
#define TERMINAL_TOKEN_DC2      ENCODE_EXEC(0x12) /* [Ctrl] R */
/* Device Control 3 (XOFF) */
#define TERMINAL_TOKEN_DC3      ENCODE_EXEC(0x13) /* [Ctrl] S */
/* Device Control 4 */
#define TERMINAL_TOKEN_DC4      ENCODE_EXEC(0x14) /* [Ctrl] T */
/* Negative acknowledge */
#define TERMINAL_TOKEN_NAK      ENCODE_EXEC(0x15) /* [Ctrl] U */
/* Synchronous Idle */
#define TERMINAL_TOKEN_SYN      ENCODE_EXEC(0x16) /* [Ctrl] V */
/* End of Transmission Block */
#define TERMINAL_TOKEN_ETB      ENCODE_EXEC(0x17) /* [Ctrl] W */
/* Cancel */
#define TERMINAL_TOKEN_CAN      ENCODE_EXEC(0x18) /* [Ctrl] X */
/* End of Medium */
#define TERMINAL_TOKEN_EM       ENCODE_EXEC(0x19) /* [Ctrl] Y */
/* Substitute */
#define TERMINAL_TOKEN_SUB      ENCODE_EXEC(0x1A) /* [Ctrl] Z */
/* Escape */
#define TERMINAL_TOKEN_ESC      ENCODE_EXEC(0x1B) /* [Ctrl] [ */
/* File Separator */
#define TERMINAL_TOKEN_FS       ENCODE_EXEC(0x1C) /* [Ctrl] \ */
/* Group Separator */
#define TERMINAL_TOKEN_GS       ENCODE_EXEC(0x1D) /* [Ctrl] ] */
/* Record Separator */
#define TERMINAL_TOKEN_RS       ENCODE_EXEC(0x1E) /* [Ctrl] ^ */
/* Unit Separator */
#define TERMINAL_TOKEN_US       ENCODE_EXEC(0x1E) /* [Ctrl] _ */
/* Delete */
#define TERMINAL_TOKEN_DEL      ENCODE_EXEC(0x7f) /* (150995071) */

/* End */
#define TERMINAL_TOKEN_END                   134217807 /* (27,79,70)(8,0,0,4F) TODO: this is temporary shit, while understand upstairs macroses */
/* Private */
#define TERMINAL_TOKEN_PRIVATE               ENCODE_CS('~')
/* Set Mode */
#define TERMINAL_TOKEN_SET_MODE              ENCODE_CS('h')
/* Reset Mode */
#define TERMINAL_TOKEN_RESET_MODE            ENCODE_CS('l')
/* Select Graphic Rendition */
#define TERMINAL_TOKEN_SGR                   ENCODE_CS('m')

#define TERMINAL_TOKEN_PARAM_MODE_LINE_WRAP     7

#define TERMINAL_TOKEN_PARAM_ERASE_DOWN_RIGHT   0
#define TERMINAL_TOKEN_PARAM_ERASE_UP_LEFT      0
#define TERMINAL_TOKEN_PARAM_ERASE_ENTIRE       2

#define TERMINAL_TOKEN_PARAM_PRIVATE_HOME	1
#define TERMINAL_TOKEN_PARAM_PRIVATE_INSERT	2
#define TERMINAL_TOKEN_PARAM_PRIVATE_DELETE	3
#define TERMINAL_TOKEN_PARAM_PRIVATE_END        4
#define TERMINAL_TOKEN_PARAM_PRIVATE_PAGE_UP    5
#define TERMINAL_TOKEN_PARAM_PRIVATE_PAGE_DOWN  6

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
	char (*getc)(void);
	void (*putc)(char ch);
} TERMINAL_IO;

/*
 * Terminal internal representation
 */
typedef struct {
	TERMINAL_IO io[1];
	struct vtbuild builder[1];
	struct vtparse parser[1];

	/* NOTE: This value is tightly relies on vtparse algorithms. */
#define VTPARSER_TOKEN_QUEUE_AMOUNT 3
	/*
	 * Saves some useful but volatile information
	 * about parser state collected at callback invocation time.
	 * NOTE: This structure is tightly relies on vtparse algorithms.
	 */
	struct vt_token vt_token_queue[VTPARSER_TOKEN_QUEUE_AMOUNT];

	int vt_token_queue_len;
	int vt_token_queue_head;

} TERMINAL;

TERMINAL *terminal_init(TERMINAL *terminal, TERMINAL_IO *io);

bool terminal_receive(TERMINAL *terminal, TERMINAL_TOKEN *token,
		short **params, int *params_len);

bool terminal_transmit(TERMINAL *terminal, TERMINAL_TOKEN token,
						short *params, int params_len);
bool terminal_transmit_va(TERMINAL *terminal, TERMINAL_TOKEN token,
						int params_len, ...);

#endif /* TERMINAL_H_ */
