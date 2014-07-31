/**
 * @file
 *
 * @date Jul 30, 2014
 * @author Denis Deryugin
 */

#ifndef VFORK_RES_NONE_H_
#define VFORK_RES_NONE_H_

#include <sys/types.h>
#include <asm/ptrace.h>

struct task_vfork {
	struct pt_regs ptregs;
};

#endif /* VFORK_RES_NONE_H_ */
