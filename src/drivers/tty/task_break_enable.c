/**
 * @file
 *
 * @date May 17, 2013
 * @author: Anton Bondarev
 */
#include <drivers/tty.h>
#include <termios.h>
#include <signal.h>

//#define TC_I(t, flag) ((t)->termios.c_iflag & (flag))
//#define TC_O(t, flag) ((t)->termios.c_oflag & (flag))
//#define TC_C(t, flag) ((t)->termios.c_cflag & (flag))
#define TTY_L(t, flag) ((t)->termios.c_lflag & (flag))

void tty_task_break_check(struct tty *t, char ch) {
	cc_t *cc = t->termios.c_cc;

	if (TTY_L(t, ISIG) && (ch == cc[VINTR]) && t->pgrp) {
		kill(t->pgrp, SIGINT);
		/*TODO normal handling when process groups will be realized*/
		t->pgrp = 0;
	}
}
