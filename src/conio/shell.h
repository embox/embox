/*
 * shell.h
 *
 *  Created on: 02.02.2009
 *      Author: Alexey Fomin
 */

#ifndef SHELL_H_
#define SHELL_H_

typedef int (*PSHELL_HANDLER)(int argsc, char **argsv);

void shell_start();

#endif /* SHELL_H_ */
