/*
 * tty.h
 *
 *  Created on: 02.02.2009
 *      Author: Eldar Abusalimov
 */

#ifndef TTY_H_
#define TTY_H_

#include "types.h"

/*
 * callback function to be fired when command would be entered
 * (e.g. after enter has been pressed)
 *
 * param: cmdline - char buffer being in use
 */
typedef void (*TTY_EXEC_CALLBACK)(char *cmdline);

typedef int (*TTY_PROPOSALS_CALLBACK)(char *cmdline, char **proposals);

/*
 * start tty interactive session
 *
 * param: callback - callback function to fire on "Enter"
 * param: welcome - welcome prompt
 */
void tty_start(TTY_EXEC_CALLBACK callback,
		TTY_PROPOSALS_CALLBACK proposals_callback, const char *welcome);

void tty_stop();

#endif /* TTY_H_ */
