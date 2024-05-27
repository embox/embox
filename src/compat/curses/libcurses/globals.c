/**
 * @file
 * @brief
 *
 * @date 1.09.22
 * @author Aleksey Zhmulin
 */

#include <termios.h>

#include "curses_priv.h"
#include "term_priv.h"

SCREEN *_curs_screen_p;

struct ctrl_seq *_curs_ctrl_seq;
struct termios _curs_shell_tty;

int _curs_currow;
int _curs_curcol;

int COLS;
int LINES;

WINDOW *curscr;
WINDOW *stdscr;

TERMINAL *cur_term;
