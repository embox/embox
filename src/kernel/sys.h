/*
 * sys.h
 *
 *  Created on: 09.06.2009
 *      Author: Eldar Abusalimov
 */

#ifndef SYS_H_
#define SYS_H_

typedef int (*EXEC_FUNC)(int argc, char **argv);

int sys_exec(EXEC_FUNC f, int argc, char **argv);
void sys_interrupt();

#endif /* SYS_H_ */
