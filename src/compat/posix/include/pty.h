/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    18.04.2014
 */

#ifndef POSIX_PTY_H_
#define POSIX_PTY_H_

#include <termios.h>

extern int openpty(int *amaster, int *aslave, char *name,
		const struct termios *termp, const struct winsize *winp);

extern pid_t forkpty(int *amaster, char *name, const struct termios *termp,
		const struct winsize *winp);

#endif /* POSIX_PTY_H_ */

