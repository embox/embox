/**
 * @file
 *
 * @date 09.06.2009
 * @author Eldar Abusalimov
 */

#ifndef SYS_H_
#define SYS_H_

#include <types.h>

typedef int (*EXEC_FUNC)(int argc, char **argv);

extern int sys_exec_start(EXEC_FUNC f, int argc, char **argv);
extern void sys_exec_stop(void);
extern bool sys_exec_is_started(void);
extern void sys_halt(void);

#endif /* SYS_H_ */
