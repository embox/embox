/**
 * @file
 * @brief
 *
 * @date 26.02.13
 * @author Ilia Vaprol
 */

#include <assert.h>
#include <curses.h>
#include <mem/misc/pool.h>
#include <string.h>
#include <util/math.h>
#include <termios.h>
#include <unistd.h>
#include <stdio.h>

/**
 * Screen info
 * TODO make SCREEN structure
 */
#define SCREEN_WIDTH  80
#define SCREEN_HEIGHT 24
static chtype std_screen[SCREEN_HEIGHT][SCREEN_WIDTH];
static chtype cur_screen[SCREEN_HEIGHT][SCREEN_WIDTH];

#define WINDOW_AMOUNT 0x10

int COLS = SCREEN_WIDTH;
int LINES = SCREEN_HEIGHT;

WINDOW *stdscr;
WINDOW *curscr;

POOL_DEF(wnd_pool, WINDOW, WINDOW_AMOUNT);

static void window_init(WINDOW *win, uint16_t begy, uint16_t begx,
		uint16_t nlines, uint16_t ncols, WINDOW *parent, chtype *lines) {
	assert(win != NULL);

	win->begy = win->cury = begy;
	win->begx = win->curx = begx;
	win->endy = win->begy + nlines;
	win->endx = win->begx + ncols;
	win->parent = parent;
	win->attrs = A_NORMAL;
	win->bkgd = (chtype)0;
	win->lines = lines;
	win->scrollok = false;
	win->clearok = false;
}

static void window_setch(WINDOW *win, chtype ch, uint16_t cury,
		uint16_t curx) {
	assert(win != NULL);
	assert(win->lines != NULL);

	*(win->lines + cury * COLS + curx) = ch;
}

static chtype window_getch(WINDOW *win, uint16_t cury, uint16_t curx) {
	assert(win != NULL);
	assert(win->lines != NULL);

	return *(win->lines + cury * COLS + curx);
}

static void window_fill(WINDOW *win, chtype ch, uint16_t begy,
		uint16_t begx, uint16_t endy, uint16_t endx) {
	uint16_t y, x;

	assert(win != NULL);

	for (y = begy; y < endy; ++y) {
		for (x = begx; x < endx; ++x) {
			window_setch(win, ch, y, x);
		}
	}
}

WINDOW * initscr(void) {
	static WINDOW standard, current;
	int res;

	stdscr = &standard;
	window_init(stdscr, 0, 0, LINES, COLS, NULL, &std_screen[0][0]);

	curscr = &current;
	window_init(curscr, 0, 0, LINES, COLS, NULL, &cur_screen[0][0]);

	memset(&std_screen[0][0], ' ', sizeof std_screen);
	memset(&cur_screen[0][0], 0, sizeof cur_screen);

	res = doupdate();
	if (res != OK) {
		return NULL;
	}

	return stdscr;
}

WINDOW * newwin(int nlines, int ncols, int begin_y, int begin_x) {
	WINDOW *win;

	if ((nlines <= 0) || (ncols <= 0)
			|| (begin_y < 0) || (begin_y >= LINES)
			|| (begin_x < 0) || (begin_x >= COLS)
			|| (begin_y + nlines > LINES)
			|| (begin_x + ncols > COLS)) {
		return NULL;
	}

	win = pool_alloc(&wnd_pool);
	if (win == NULL) {
		return NULL;
	}

	window_init(win, begin_y, begin_x,
			nlines != 0 ? nlines : LINES - begin_y,
			ncols != 0 ? ncols : COLS - begin_x, NULL,
			stdscr->lines);

	return win;
}

WINDOW * subwin(WINDOW *orig, int nlines, int ncols, int begin_y,
		int begin_x) {
	WINDOW *win;

	if ((orig == NULL) || (nlines <= 0) || (ncols <= 0)
			|| (begin_y < orig->begy) || (begin_y >= orig->endy)
			|| (begin_x < orig->begx) || (begin_x >= orig->endx)
			|| (begin_y + nlines > orig->endy)
			|| (begin_x + ncols > orig->endx)) {
		return NULL;
	}

	win = pool_alloc(&wnd_pool);
	if (win == NULL) {
		return NULL;
	}

	window_init(win, begin_y, begin_x, nlines, ncols, orig, stdscr->lines);

	return win;
}

WINDOW * derwin(WINDOW *orig, int nlines, int ncols, int begin_y,
		int begin_x) {
	return subwin(orig, nlines, ncols, orig->begy + begin_y,
			orig->begx + begin_x);
}

int delwin(WINDOW *win) {
	pool_free(&wnd_pool, win);

	return OK;
}

int doupdate(void) {
	chtype ch;
	uint16_t y, x;

	for (y = 0; y < LINES; ++y) {
		for (x = 0; x < COLS; ++x) {
			ch = window_getch(stdscr, y, x);
			if (ch != window_getch(curscr, y, x)) {
				printf("\x1b[%hu;%huH%c", y + 1, x + 1,
						(char)window_getch(stdscr, y, x));
			}
		}
	}

	memcpy(&cur_screen[0][0], &std_screen[0][0], sizeof cur_screen);

	return OK;
}

int refresh(void) {
	return wrefresh(stdscr);
}

int wnoutrefresh(WINDOW *win) {
	return OK;
}

int wrefresh(WINDOW *win) {
	int res;

	if (win == NULL) {
		return ERR;
	}

	if (curscr->clearok || win->clearok) {
		wclear(win);
		curscr->clearok = win->clearok = false;
	}

	res = wnoutrefresh(win);
	if (res != OK) {
		return res;
	}

	return doupdate();
}

int move(int y, int x) {
	return wmove(stdscr, y, x);
}

int wmove(WINDOW *win, int y, int x) {
	if ((win == NULL)
			|| (y < 0) || (y >= win->endy - win->begy)
			|| (x < 0) || (x >= win->endx - win->begx)) {
		return ERR;
	}

	win->cury = win->begy + y;
	win->curx = win->begx + x;

	return OK;
}

int addch(const chtype ch) {
	return waddch(stdscr, ch);
}

int mvaddch(int y, int x, const chtype ch) {
	return mvwaddch(stdscr, y, x, ch);
}

int mvwaddch(WINDOW *win, int y, int x, const chtype ch) {
	int res;

	res = wmove(win, y, x);
	if (res != OK) {
		return res;
	}

	return waddch(win, ch);
}

int waddch(WINDOW *win, const chtype ch) {
	if (win == NULL) {
		return ERR;
	}

	window_setch(win, ch, win->cury, win->curx);

	++win->curx;
	if (win->curx == win->endx) {
		win->curx = win->begx;

		++win->cury;
		if (win->cury == win->endy) {
			win->cury = win->begy;
		}
	}

	return OK;
}

int addnstr(const char *str, int n) {
	return waddnstr(stdscr, str, n);
}

int addstr(const char *str) {
	return waddstr(stdscr, str);
}

int mvaddnstr(int y, int x, const char *str, int n) {
	return mvwaddnstr(stdscr, y, x, str, n);
}

int mvaddstr(int y, int x, const char *str) {
	return mvwaddstr(stdscr, y, x, str);
}

int mvwaddnstr(WINDOW *win, int y, int x, const char *str, int n) {
	int res;

	res = wmove(win, y, x);
	if (res != OK) {
		return res;
	}

	return waddnstr(win, str, n);
}

int mvwaddstr(WINDOW *win, int y, int x, const char *str) {
	return mvwaddnstr(win, y, x, str, -1);
}

int waddnstr(WINDOW *win, const char *str, int n) {
	int res;
	size_t str_len;

	if ((win == NULL) || (str == NULL)
			|| ((n < 0) && (n != -1))) {
		return ERR;
	}

	str_len = strlen(str);
	n = n == -1 ? strlen(str) : min(n, str_len);

	while (n-- > 0) {
		res = waddch(win, *str++);
		if (res != OK) {
			return res;
		}
	}

	return OK;
}

int waddstr(WINDOW *win, const char *str) {
	return waddnstr(win, str, -1);
}

int scrl(int n) {
	return wscrl(stdscr, n);
}

int scroll(WINDOW *win) {
	return wscrl(win, 1);
}

int wscrl(WINDOW *win, int n) {
	size_t src_line, dst_line;
	chtype *src, *dst;

	if (!win->scrollok) {
		return OK;
	}

	if (n > 0) {
		src_line = win->begy + n;
		dst_line = win->begy;

		while (src_line < win->endy) {
			src = win->lines + src_line * COLS + win->begx;
			dst = win->lines + dst_line * COLS + win->begx;
			memcpy(dst, src, (win->endx - win->begx) * sizeof(chtype));
			++src_line;
			++dst_line;
		}

		window_fill(win, win->bkgd, dst_line, win->begx, win->endy, win->endx);
	}
	else {
		n = min(-n, win->endy - win->begy);
		src_line = win->endy - n;
		dst_line = win->endy;

		while (src_line >= win->begy) {
			src = win->lines + src_line * COLS + win->begx;
			dst = win->lines + dst_line * COLS + win->begx;
			memcpy(dst, src, (win->endx - win->begx) * sizeof(chtype));
			--src_line;
			--dst_line;
		}

		window_fill(win, win->bkgd, win->endy - n, win->begx, win->endy, win->endx);
	}

	return OK;
}

int scrollok(WINDOW *win, bool bf) {
	if (win == NULL) {
		return ERR;
	}

	win->scrollok = bf;

	return OK;
}

int clearok(WINDOW *win, bool bf) {
	if (win == NULL) {
		return ERR;
	}

	win->clearok = bf;

	return OK;
}

int clear(void) {
	return wclear(stdscr);
}

int erase(void) {
	return clear();
}

int wclear(WINDOW *win) {
	if (win == NULL) {
		return ERR;
	}

	window_fill(win, win->bkgd, win->begy, win->begx, win->endy, win->endx);

	return OK;
}

int werase(WINDOW *win) {
	return wclear(win);
}

int attroff(int attrs) {
	return wattroff(stdscr, attrs);
}

int attron(int attrs) {
	return wattron(stdscr, attrs);
}

int attrset(int attrs) {
	return wattrset(stdscr, attrs);
}

int wattroff(WINDOW *win, int attrs) {
	if (win == NULL) {
		return ERR;
	}

	win->attrs &= ~(attr_t)attrs;

	return OK;
}

int wattron(WINDOW *win, int attrs) {
	if (win == NULL) {
		return ERR;
	}

	win->attrs |= (attr_t)attrs;

	return OK;
}

int wattrset(WINDOW *win, int attrs) {
	if (win == NULL) {
		return ERR;
	}

	win->attrs = (attr_t)attrs;

	return OK;
}

int insch(chtype ch) {
	return winsch(stdscr, ch);
}

int mvinsch(int y, int x, chtype ch) {
	return mvwinsch(stdscr, y, x, ch);
}

int mvwinsch(WINDOW *win, int y, int x, chtype ch) {
	if (win == NULL) {
		return ERR;
	}

	window_setch(win, ch, y, x);

	return OK;
}

int winsch(WINDOW *win, chtype ch) {
	if (win == NULL) {
		return ERR;
	}

	return mvwinsch(win, win->cury, win->curx, ch);
}

int delch(void) {
	return wdelch(stdscr);
}

int mvdelch(int y, int x) {
	return mvwdelch(stdscr, y, x);
}

int mvwdelch(WINDOW *win, int y, int x) {
	if (win == NULL) {
		return ERR;
	}

	window_setch(win, win->bkgd, y, x);

	return OK;
}

int wdelch(WINDOW *win) {
	if (win == NULL) {
		return ERR;
	}

	return mvwdelch(win, win->cury, win->curx);
}

int getch(void) {
	return wgetch(stdscr);
}

int mvgetch(int y, int x) {
	return mvwgetch(stdscr, y, x);
}

int mvwgetch(WINDOW *win, int y, int x) {
	int res;

	res = wmove(win, y, x);
	if (res != OK) {
		return res;
	}

	return wgetch(win);
}

int wgetch(WINDOW *win) {
	int c;

	printf("\x1b[%d;%dH", win->cury + 1, win->curx + 1); /* FIXME setup cursor*/

	c = getchar();
	if (c == '\n') {
		return KEY_ENTER;
	}

	return c;
}

int bkgd(chtype ch) {
	return wbkgd(stdscr, ch);
}

void bkgdset(chtype ch) {
	wbkgdset(stdscr, ch);
}

chtype getbkgd(WINDOW *win) {
	assert(win != NULL);
	return win->bkgd;
}

int wbkgd(WINDOW *win, chtype ch) {
	uint16_t y, x;
	chtype old_ch;

	assert(win != NULL);

	old_ch = getbkgd(win);
	wbkgdset(win, ch);

	for (y = win->begy; y < win->endy; ++y) {
		for (x = win->begx; x < win->endx; ++x) {
			if (window_getch(win, y, x) == old_ch) {
				window_setch(win, ch, y, x);
			}
		}
	}

	return OK;
}

void wbkgdset(WINDOW *win, chtype ch) {
	assert(win != NULL);
	win->bkgd = ch;
}

int cbreak(void) {
    struct termios t;

	if (-1 == tcgetattr(STDIN_FILENO, &t)) {
		return ERR;
	}

	t.c_lflag &= ~ICANON;

	return -1 != tcsetattr(STDIN_FILENO, TCSANOW, &t) ? OK : ERR;
}

int nocbreak(void) {
    struct termios t;

	if (-1 == tcgetattr(STDIN_FILENO, &t)) {
		return ERR;
	}

	t.c_lflag |= ICANON;

	return -1 != tcsetattr(STDIN_FILENO, TCSANOW, &t) ? OK : ERR;
}

int raw(void) {
    struct termios t;

	if (-1 == tcgetattr(STDIN_FILENO, &t)) {
		return ERR;
	}

	t.c_iflag &= ~IXON;
	t.c_lflag &= ~(ICANON | ISIG | IXON);

	return -1 != tcsetattr(STDIN_FILENO, TCSANOW, &t) ? OK : ERR;
}

int noraw(void) {
    struct termios t;

	if (-1 == tcgetattr(STDIN_FILENO, &t)) {
		return ERR;
	}

	t.c_iflag |= IXON;
	t.c_lflag |= ICANON | ISIG;

	return -1 != tcsetattr(STDIN_FILENO, TCSANOW, &t) ? OK : ERR;
}

int echo(void) {
    struct termios t;

	if (-1 == tcgetattr(STDIN_FILENO, &t)) {
		return ERR;
	}

	t.c_lflag |= ECHO;

	return -1 != tcsetattr(STDIN_FILENO, TCSANOW, &t) ? OK : ERR;
}

int noecho(void) {
    struct termios t;

	if (-1 == tcgetattr(STDIN_FILENO, &t)) {
		return ERR;
	}

	t.c_lflag &= ~ECHO;

	return -1 != tcsetattr(STDIN_FILENO, TCSANOW, &t) ? OK : ERR;
}

int nl(void) {
    struct termios t;

	if (-1 == tcgetattr(STDIN_FILENO, &t)) {
		return ERR;
	}

	t.c_iflag |= ICRNL;

	return -1 != tcsetattr(STDIN_FILENO, TCSANOW, &t) ? OK : ERR;
}

int nonl(void) {
    struct termios t;

	if (-1 == tcgetattr(STDIN_FILENO, &t)) {
		return ERR;
	}

	t.c_iflag &= ~ICRNL;

	return -1 != tcsetattr(STDIN_FILENO, TCSANOW, &t) ? OK : ERR;
}
#include <fcntl.h>
int nodelay(WINDOW *win, bool bf) {
    struct termios t;
//TODO now we setup nonblock mode tty by the fcntl
	fcntl(STDIN_FILENO, F_SETFD, bf ? O_NONBLOCK : 0);

	if (-1 == tcgetattr(STDIN_FILENO, &t)) {
		return ERR;
	}

    t.c_cc[VMIN] = bf ? 0 : 1;
	t.c_cc[VTIME] = 0;

	return -1 != tcsetattr(STDIN_FILENO, TCSANOW, &t) ? OK : ERR;
}
