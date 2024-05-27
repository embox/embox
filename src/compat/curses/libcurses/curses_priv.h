/**
 * @file
 * @brief
 *
 * @date 1.09.22
 * @author Aleksey Zhmulin
 */

#ifndef CURSES_PRIV_H_
#define CURSES_PRIV_H_

#include <curses.h>
#include <termios.h>

#include "term_priv.h"

#define SCREEN_MAX_HEIGHT 200
#define SCREEN_MAX_WIDTH  200

#define SCREEN_HEIGHT     24
#define SCREEN_WIDTH      80

#define OUTPUT_BUF_SIZE   2144
#define INPUT_FIFO_SIZE   256

#define WIN_KEYPAD        0x001 /* Scrolling ok. */
#define WIN_NODELAY       0x002 /* Input delay. */
#define WIN_FULLWIN       0x004 /* Window is a screen. */
#define WIN_CLEAROK       0x008 /* Clear on next refresh. */
#define WIN_LEAVEOK       0x010 /* If curser left */
#define WIN_SCROLLOK      0x020 /* Scrolling ok. */

#define NL_MODE           0x001
#define RAW_MODE          0x002
#define ECHO_MODE         0x004
#define DELAY_MODE        0x008
#define CBREAK_MODE       0x010
#define CURSES_MODE       0x020

#define NOCHANGE          (curs_size_t)(-1) /* no change on this line */

struct line {
	chtype *text;
	curs_size_t firstchar;
	curs_size_t lastchar;
};

typedef struct screen {
	TERMINAL *term;

	FILE *ofp;
	FILE *ifp;

	int lines;
	int cols;

	WINDOW *std_win;
	WINDOW *cur_win;

	struct termios prog_tty;

	uint16_t flags;
#undef SCREEN
} SCREEN;

struct ctrl_seq {
	struct ctrl_seq *sibling;
	struct ctrl_seq *child;
	int value;
	char ch;
};

extern SCREEN *_curs_screen_p;
#define SP _curs_screen_p

extern struct ctrl_seq *_curs_ctrl_seq;

extern struct termios _curs_shell_tty;

extern int _curs_currow;
extern int _curs_curcol;

#define _curs_putp(str) tputs(str, 1, _curs_putc)
#define _curs_mvcur(newrow, newcol)                    \
	mvcur(_curs_currow, _curs_curcol, newrow, newcol); \
	_curs_currow = newrow;                             \
	_curs_curcol = newcol

extern int _curs_ctrl_seq_init(void);

extern int _curs_putc(int ch);
extern int _curs_puts(void *str, size_t len);
extern int _curs_flush(void);

#endif /* CURSES_PRIV_H_ */
