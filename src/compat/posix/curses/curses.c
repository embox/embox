/**
 * @file
 * @brief http://pubs.opengroup.org/onlinepubs/7908799/xcurses/curses.h.html
 *
 * @date 26.02.13
 * @author Ilia Vaprol
 */

#include <assert.h>
#include <curses.h>
#include <errno.h>
#include <string.h>
#include <termios.h>
#include <stdio.h>
#include <inttypes.h>

#include <util/math.h>
#include <lib/libds/array.h>
#include <mem/misc/pool.h>

int COLS;
int LINES;

WINDOW *stdscr;
WINDOW *curscr;

#define WINDOW_AMOUNT 0x10

#define SCREEN_WIDTH  80
#define SCREEN_HEIGHT 24

#define SCREEN_MAX_WIDTH  200
#define SCREEN_MAX_HEIGHT 200

struct line {
	chtype *text;
	curs_size_t firstchar;
	curs_size_t lastchar;
};

struct screen {
	char *type;
	FILE *out;
	FILE *in;
	WINDOW std_win;   /* stdscr storage */
	chtype std_buff[SCREEN_MAX_HEIGHT][SCREEN_MAX_WIDTH];
	WINDOW cur_win;   /* curscr storage */
	chtype cur_buff[SCREEN_MAX_HEIGHT][SCREEN_MAX_WIDTH];
	bool curses_mode; /* terminal mode (normal or curses) */
    struct termios last_mode;
};

static SCREEN screen;

POOL_DEF(wnd_pool, WINDOW, WINDOW_AMOUNT);
POOL_DEF(line_pool, struct line, WINDOW_AMOUNT);

static void window_init(WINDOW *win, uint16_t begy, uint16_t begx,
		uint16_t nlines, uint16_t ncols, WINDOW *parent, chtype *lines) {
	assert(win != NULL);

	win->lines = pool_alloc(&line_pool);
	if (win->lines == NULL) {
		return;
	}

	win->begy = win->cury = begy;
	win->begx = win->curx = begx;
	win->maxy = win->begy + nlines;
	win->maxx = win->begx + ncols;
	win->parent = parent;
	win->attrs = A_NORMAL;
	win->bkgd = (chtype)0;
	win->lines->text = lines;
	win->scrollok = false;
	win->clearok = false;
}

static void window_setch(WINDOW *win, chtype ch, uint16_t cury,
		uint16_t curx) {
	assert(win != NULL);
	assert(win->lines != NULL);
	assert((cury >= win->begy) && (cury < win->maxy));
	assert((curx >= win->begx) && (cury < win->maxx));

	*(win->lines->text + cury * COLS + curx) = ch;
}

static chtype window_getch(WINDOW *win, uint16_t cury, uint16_t curx) {
	assert(win != NULL);
	assert(win->lines != NULL);
	assert((cury >= win->begy) && (cury < win->maxy));
	assert((curx >= win->begx) && (cury < win->maxx));

	return *(win->lines->text + cury * COLS + curx);
}

static void window_fill(WINDOW *win, chtype ch, uint16_t begy,
		uint16_t begx, uint16_t maxy, uint16_t maxx) {
	uint16_t y, x;

	assert(win != NULL);

	for (y = begy; y < maxy; ++y) {
		for (x = begx; x < maxx; ++x) {
			window_setch(win, ch, y, x);
		}
	}
}

SCREEN *newterm(char *type, FILE *outfile, FILE *infile) {
	int res;

	COLS = SCREEN_WIDTH;
	LINES = SCREEN_HEIGHT;

	screen.type = type;
	screen.out = outfile;
	screen.in = infile;
	window_init(&screen.std_win, 0, 0, LINES, COLS, NULL, &screen.std_buff[0][0]);
	memset(&screen.std_buff[0][0], 0, sizeof screen.std_buff);
	window_init(&screen.cur_win, 0, 0, LINES, COLS, NULL, &screen.cur_buff[0][0]);
	memset(&screen.cur_buff[0][0], 0, sizeof screen.cur_buff);
	screen.curses_mode = true;

	if (-1 == tcgetattr(fileno(screen.in), &screen.last_mode)) {
		return NULL;
	}

	stdscr = &screen.std_win;
	curscr = &screen.cur_win;

	res = doupdate();
	if (res != OK) {
		return NULL;
	}

	return &screen;
}

static int screen_change_mode(void) {
	struct termios t;

	if (-1 == tcgetattr(fileno(screen.in), &t)) {
		return -errno;
	}

	if (-1 == tcsetattr(fileno(screen.in), TCSANOW,
				&screen.last_mode)) {
		return -errno;
	}

	memcpy(&screen.last_mode, &t, sizeof screen.last_mode);

	screen.curses_mode = !screen.curses_mode;

	return 0;
}

WINDOW *initscr(void) {
	if (NULL == newterm("", stdout, stdin)) {
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
			stdscr->lines->text);

	return win;
}

WINDOW * subwin(WINDOW *orig, int nlines, int ncols, int begin_y,
		int begin_x) {
	WINDOW *win;

	if ((orig == NULL) || (nlines <= 0) || (ncols <= 0)
			|| (begin_y < orig->begy) || (begin_y >= orig->maxy)
			|| (begin_x < orig->begx) || (begin_x >= orig->maxx)
			|| (begin_y + nlines > orig->maxy)
			|| (begin_x + ncols > orig->maxx)) {
		return NULL;
	}

	win = pool_alloc(&wnd_pool);
	if (win == NULL) {
		return NULL;
	}

	window_init(win, begin_y, begin_x, nlines, ncols, orig, stdscr->lines->text);

	return win;
}

WINDOW * derwin(WINDOW *orig, int nlines, int ncols, int begin_y,
		int begin_x) {
	return subwin(orig, nlines, ncols, orig->begy + begin_y,
			orig->begx + begin_x);
}

int delwin(WINDOW *win) {
	pool_free(&line_pool, win->lines);
	pool_free(&wnd_pool, win);

	return OK;
}

int doupdate(void) {
	chtype *std_ptr, *cur_ptr, *std_end;
	ptrdiff_t offset;
	uint16_t x;
	/* uint16_t y */

	if (isendwin()) {
		if (0 != screen_change_mode()) {
			return ERR;
		}
	}

	std_ptr = stdscr->lines->text;
	cur_ptr = curscr->lines->text;
	std_end = stdscr->lines->text + LINES * COLS;

	do {
		for (; (std_ptr < std_end) && (*std_ptr == *cur_ptr);
				++std_ptr, ++cur_ptr) { }

		offset = std_ptr - stdscr->lines->text;
		/* y = offset / COLS; */
		x = offset % COLS;

		/* fprintf(screen.out, "\x1b[%" PRIu16 ";%" PRIu16 "H", y + 1, x + 1); TODO cursor position */

		for (; (std_ptr < std_end) && (*std_ptr != *cur_ptr);
				++std_ptr, ++cur_ptr) {
			*cur_ptr = *std_ptr;
			fprintf(screen.out, "%c", (char)*std_ptr);
			if (++x == COLS) {
				fprintf(screen.out, "\n");
				x = 0;
			}
		}
	} while (std_ptr < std_end);

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
		res = wclear(win);
		if (res != OK) {
			return res;
		}
		curscr->clearok = win->clearok = false;
	}

	res = wnoutrefresh(win);
	if (res != OK) {
		return res;
	}

	return doupdate();
}

bool isendwin(void) {
	return !screen.curses_mode;
}

int endwin(void) {
	uint16_t y, x;

	if (isendwin()) {
		return ERR;
	}

	if (0 != screen_change_mode()) {
		return ERR;
	}

	/* fprintf(screen.out, "\x1b[1;1H"); TODO cursor position  */
	for (y = 0; y < LINES; ++y) {
		for (x = 0; x < COLS; ++x) {
			window_setch(curscr, ' ', y, x);
			fprintf(screen.out, " ");
		}
	}

	/* fprintf(screen.out, "\x1b[1;1H"); TODO cursor position */

	return OK;
}

int move(int y, int x) {
	return wmove(stdscr, y, x);
}

int wmove(WINDOW *win, int y, int x) {
	if ((win == NULL)
			|| (y < 0) || (y >= win->maxy - win->begy)
			|| (x < 0) || (x >= win->maxx - win->begx)) {
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
	int res;

	if (win == NULL) {
		return ERR;
	}

	window_setch(win, ch, win->cury, win->curx);

	++win->curx;
	if (win->curx == win->maxx) {
		win->curx = win->begx;

		++win->cury;
		if (win->cury == win->maxy) {
			win->cury = win->begy;
		}

		if (win->scrollok) {
			res = scroll(win);
			if (res != OK) {
				return res;
			}
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
		return ERR;
	}

	if (n > 0) {
		src_line = win->begy + n;
		dst_line = win->begy;

		while (src_line < win->maxy) {
			src = win->lines->text + src_line * COLS + win->begx;
			dst = win->lines->text + dst_line * COLS + win->begx;
			memcpy(dst, src, (win->maxx - win->begx) * sizeof(chtype));
			++src_line;
			++dst_line;
		}

		window_fill(win, win->bkgd, dst_line, win->begx, win->maxy, win->maxx);
	}
	else {
		n = min(-n, win->maxy - win->begy);
		src_line = win->maxy - n - 1;
		dst_line = win->maxy - 1;

		while (src_line >= win->begy) {
			src = win->lines->text + src_line * COLS + win->begx;
			dst = win->lines->text + dst_line * COLS + win->begx;
			memcpy(dst, src, (win->maxx - win->begx) * sizeof(chtype));
			--src_line;
			--dst_line;
		}

		window_fill(win, win->bkgd, win->begy, win->begx, src_line, win->maxx);
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

	window_fill(win, win->bkgd, win->begy, win->begx, win->maxy, win->maxx);

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

	window_setch(win, ch, win->cury + y, win->curx + x);

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

	window_setch(win, win->bkgd, win->cury + y, win->curx + x);

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

	/* fprintf(screen.out, "\x1b[%d;%dH", win->cury + 1, win->curx + 1);  FIXME setup cursor*/

	c = fgetc(screen.in);
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

	for (y = win->begy; y < win->maxy; ++y) {
		for (x = win->begx; x < win->maxx; ++x) {
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

	if (-1 == tcgetattr(fileno(screen.in), &t)) {
		return ERR;
	}

	t.c_lflag &= ~ICANON;

	return -1 != tcsetattr(fileno(screen.in), TCSANOW, &t) ? OK : ERR;
}

int nocbreak(void) {
    struct termios t;

	if (-1 == tcgetattr(fileno(screen.in), &t)) {
		return ERR;
	}

	t.c_lflag |= ICANON;

	return -1 != tcsetattr(fileno(screen.in), TCSANOW, &t) ? OK : ERR;
}

int raw(void) {
    struct termios t;

	if (-1 == tcgetattr(fileno(screen.in), &t)) {
		return ERR;
	}

	t.c_iflag &= ~IXON;
	t.c_lflag &= ~(ICANON | ISIG | IXON);

	return -1 != tcsetattr(fileno(screen.in), TCSANOW, &t) ? OK : ERR;
}

int noraw(void) {
    struct termios t;

	if (-1 == tcgetattr(fileno(screen.in), &t)) {
		return ERR;
	}

	t.c_iflag |= IXON;
	t.c_lflag |= ICANON | ISIG;

	return -1 != tcsetattr(fileno(screen.in), TCSANOW, &t) ? OK : ERR;
}

int echo(void) {
    struct termios t;

	if (-1 == tcgetattr(fileno(screen.in), &t)) {
		return ERR;
	}

	t.c_lflag |= ECHO;

	return -1 != tcsetattr(fileno(screen.in), TCSANOW, &t) ? OK : ERR;
}

int noecho(void) {
    struct termios t;

	if (-1 == tcgetattr(fileno(screen.in), &t)) {
		return ERR;
	}

	t.c_lflag &= ~ECHO;

	return -1 != tcsetattr(fileno(screen.in), TCSANOW, &t) ? OK : ERR;
}

int nl(void) {
    struct termios t;

	if (-1 == tcgetattr(fileno(screen.in), &t)) {
		return ERR;
	}

	t.c_iflag |= ICRNL;

	return -1 != tcsetattr(fileno(screen.in), TCSANOW, &t) ? OK : ERR;
}

int nonl(void) {
    struct termios t;

	if (-1 == tcgetattr(fileno(screen.in), &t)) {
		return ERR;
	}

	t.c_iflag &= ~ICRNL;

	return -1 != tcsetattr(fileno(screen.in), TCSANOW, &t) ? OK : ERR;
}

int nodelay(WINDOW *win, bool bf) {
    struct termios t;

	if (-1 == tcgetattr(fileno(screen.in), &t)) {
		return ERR;
	}

	t.c_cc[VMIN] = bf ? 0 : 1;
	t.c_cc[VTIME] = 0;

	return -1 != tcsetattr(fileno(screen.in), TCSANOW, &t) ? OK : ERR;
}

int curs_set(int visibility) {
	return OK;
}
/****common part*****/
int beep(void) {
	return OK;
}

int keypad(WINDOW *win, bool bf) {
#if 0
	if (/*(SP == NULL)*/ || (win == NULL)) {
		return ERR;
	}

	if (bf) {
		win->flags |= WIN_KEYPAD;
	}
	else {
		win->flags &= ~WIN_KEYPAD;
	}
#endif
	return OK;
}

