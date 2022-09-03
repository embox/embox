/**
 * @file
 * @brief
 *
 * @date 1.09.22
 * @author Aleksey Zhmulin
 */

#include <curses_priv.h>

#include <termios.h>

SCREEN *SP;

struct ctrl_seq *_curs_ctrl_seq;

struct termios _curs_shell_tty;

int _curs_currow;
int _curs_curcol;
