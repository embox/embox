/**
 * @file
 * @brief
 *
 * @date 1.09.22
 * @author Aleksey Zhmulin
 */

#include <curses_priv.h>

#include <stdlib.h>
#include <util/math.h>

WINDOW *initscr(void) {
	if (!_curs_init_screen) {
		if (NULL == newterm(NULL, stdout, stdin)) {
			return NULL;
		}
		_curs_init_screen = TRUE;
	}

	return stdscr;
}

int endwin(void) {
	if (isendwin()) {
		return ERR;
	}

	_curs_putp(TINFO_EXIT_CA_MODE);
	_curs_flush();

	reset_shell_mode();

	SP->flags &= ~CURSES_MODE;

	return OK;
}

bool isendwin(void) {
	if (SP == NULL) {
		return TRUE;
	}

	return !(SP->flags & CURSES_MODE);
}

SCREEN *newterm(char *type, FILE *ofp, FILE *ifp) {
	if ((cur_term == NULL) || (type != cur_term->name)) {
		setupterm(type, fileno(ofp), NULL);
	}
	
	if (ERR == _curs_ctrl_seq_init()) {
		return NULL;
	}

	SP = malloc(sizeof(SCREEN));
	if (SP == NULL) {
		return NULL;
	}

	LINES = TINFO_LINES;
	if (LINES <= 0) {
		LINES = SCREEN_HEIGHT;
	}

	COLS = TINFO_COLUMNS;
	if (COLS <= 0) {
		COLS = SCREEN_WIDTH;
	}

	LINES = min(LINES, SCREEN_MAX_HEIGHT);
	COLS = min(COLS, SCREEN_MAX_WIDTH);

	stdscr = newwin(LINES, COLS, 0, 0);
	curscr = newwin(LINES, COLS, 0, 0);

	if ((stdscr == NULL) || (curscr == NULL)) {
		return NULL;
	}

	SP->term = cur_term;
	SP->ofp = ofp;
	SP->ifp = ifp;
	SP->lines = LINES;
	SP->cols = COLS;
	SP->std_win = stdscr;
	SP->cur_win = curscr;
	SP->flags |= CURSES_MODE;

	def_prog_mode();
	def_shell_mode();

	SP->prog_tty.c_lflag &= ~(ECHO | ECHONL);
	SP->prog_tty.c_iflag &= ~(ICRNL | INLCR | IGNCR);
	SP->prog_tty.c_oflag &= ~(ONLCR);

	raw();
	echo();

	_curs_putp(TINFO_ENTER_CA_MODE);
	_curs_putp(TINFO_CURSOR_HOME);

	return SP;
}

void delscreen(SCREEN *SP) {
	if (SP == NULL) {
		return;
	}

	delwin(SP->std_win);
	delwin(SP->cur_win);

	free(SP);
}
