/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    15.05.2014
 */

#ifndef HAL_VFORK_H_
#define HAL_VFORK_H_

#include <sys/types.h> /* pid_t */

extern pid_t fork(void);

struct pt_regs;
extern void __attribute__((noreturn)) vfork_leave(struct pt_regs *ptregs);

struct task;
extern void vfork_child_done(struct task *task, void * (*run)(void *), void *arg);

#endif /* HAL_VFORK_H_ */

