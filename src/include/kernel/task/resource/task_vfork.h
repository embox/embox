/**
 * @file
 *
 * @date May 16, 2014
 * @author: Anton Bondarev
 */

#ifndef TASK_VFORK_H_
#define TASK_VFORK_H_

#include <setjmp.h>
#include <sys/types.h>

struct task_vfork {
	jmp_buf vfrok_jmpbuf;
	pid_t vforked_pid;
};

struct task;

extern struct task_vfork *task_resource_vfork(const struct task *task);

#endif /* TASK_VFORK_H_ */
