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
#include <setjmp.h>
#include <signal.h>

extern int nano_main(int argc, char **argv);

static inline int sigfillset(sigset_t *set) { return -1; }
static inline int sigaction(int sig, const struct sigaction *act,
		struct sigaction *oact) { return -1; }

#endif /* EMBOX_H_ */
