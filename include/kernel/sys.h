/**
 * \file sys.h
 *
 * \date 09.06.2009
 * \author Eldar Abusalimov
 */

#ifndef SYS_H_
#define SYS_H_

#include "types.h"
#include "asm/cpu_context.h"

typedef int (*EXEC_FUNC)(int argc, char **argv);

void context_save(CPU_CONTEXT * pcontext);
void context_restore(CPU_CONTEXT * pcontext);
int sys_exec_start(EXEC_FUNC f, int argc, char **argv);
void sys_exec_stop();
int sys_exec_is_started();
void sys_halt();

#endif /* SYS_H_ */
