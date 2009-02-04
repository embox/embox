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
 * param: entered - char buffer being in use
 *
 * return: new welcome prompt
 */
typedef char* (*TTY_CALLBACK)(const char *entered);

void tty_start(TTY_CALLBACK c);

#endif /* TTY_H_ */
