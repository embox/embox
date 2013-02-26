/**
 * @file
 * @brief
 *
 * @date 26.02.13
 * @author Ilia Vaprol
 */

#include <curses.h>

int COLS;
int LINES;
WINDOW *curscr;

int beep(void) { return 0; }
int wrefresh(WINDOW *win) { return 0; }
int endwin(void) { return 0; }
int delwin(WINDOW *win) { return 0; }
WINDOW *newwin(int nlines, int ncols, int begin_y, int begin_x) { static WINDOW tmp; return &tmp; }
int keypad(WINDOW *win, bool bf) { return 0; }
int doupdate(void) { return 0; }
int move(int y, int x) { return 0; }
int curs_set(int visibility) { return 0; }
int wnoutrefresh(WINDOW *win) { return 0; }
int raw(void) { return 0; }
int nonl(void) { return 0; }
int noecho(void) { return 0; }
WINDOW *initscr(void) { static WINDOW tmp; return &tmp; }
int wmove(WINDOW *win, int y, int x) { return 0; }
int wattron(WINDOW *win, int attrs) { return 0; }
int mvwaddnstr(WINDOW *win, int y, int x, const char *str, int n) { return 0; }
int waddch(WINDOW *win, const chtype ch) { return 0; }
int waddstr(WINDOW *win, const char *str) { return 0; }
int wattroff(WINDOW *win, int attrs) { return 0; }
int wgetch(WINDOW *win) { return 0; }
int nodelay(WINDOW *win, bool bf) { return 0; }
int waddnstr(WINDOW *win, const char *str, int n) { return 0; }
bool isendwin(void) { return true; }
int mvwaddstr(WINDOW *win, int y, int x, const char *str) { return 0; }
int mvwaddch(WINDOW *win, int y, int x, const chtype ch) { return 0; }
int scrollok(WINDOW *win, bool bf) { return 0; }
int wscrl(WINDOW *win, int n) { return 0; }
