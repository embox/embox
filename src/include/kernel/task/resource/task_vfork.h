/**
 * @file
 *
 * @date May 16, 2014
 * @author: Anton Bondarev
 */

#ifndef TASK_VFORK_H_
#define TASK_VFORK_H_

#include <sys/types.h>
#include <asm/ptrace.h>

struct task;

struct task_vfork {
	struct pt_regs ptregs;
	//pid_t vforked_pid;
	//struct task *vforked_task;
	//char cmdline[80];
};

extern struct task_vfork *task_resource_vfork(const struct task *task);

#endif /* TASK_VFORK_H_ */
