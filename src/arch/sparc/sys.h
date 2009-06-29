/**
 * \file sys.h
 *
 * \date 09.06.2009
 * \author Eldar Abusalimov
 */

#ifndef SYS_H_
#define SYS_H_

#include "cpu_context.h"
#include "types.h"

typedef int (*EXEC_FUNC)(int argc, char **argv);

void context_save(CPU_CONTEXT * pcontext);
void context_restore(CPU_CONTEXT * pcontext);
int sys_exec(EXEC_FUNC f, int argc, char **argv);
void sys_interrupt();

#endif /* SYS_H_ */
