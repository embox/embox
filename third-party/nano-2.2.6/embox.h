/**
 * @file
 * @brief
 *
 * @date 08.03.13
 * @author Ilia Vaprol
 */

#ifndef EMBOX_H_
#define EMBOX_H_

#include <curses.h>
#include <stdlib.h>
#include <setjmp.h>

extern int nano_exit_ret;
extern int nano_exited;
extern jmp_buf nano_exit_point;
extern int nano_main(int argc, char **argv);

#define NANO_EXIT_JMP_VALUE 177

#define exit(status) \
	do { \
		nano_exit_ret = status; \
		nano_exited = 1; \
		longjmp(nano_exit_point, NANO_EXIT_JMP_VALUE); \
	} while(0);

static inline int beep(void) { return 0; }
static inline int endwin(void) { return 0; }
static inline int keypad(WINDOW *win, bool bf) { return 0; }
static inline int curs_set(int visibility) { return 0; }
static inline int raw(void) { return 0; }
static inline int nonl(void) { return 0; }
static inline int noecho(void) { return 0; }
static inline int nodelay(WINDOW *win, bool bf) { return 0; }
static inline bool isendwin(void) { return TRUE; }

#endif /* EMBOX_H_ */
