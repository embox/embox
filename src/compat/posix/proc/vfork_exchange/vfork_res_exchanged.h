/**
 * @file
 *
 * @date Jul 12, 2014
 * @author Anton Bondarev
 */

#ifndef VFORK_RES_EXCHANGED_H_
#define VFORK_RES_EXCHANGED_H_

#include <sys/types.h>
#include <asm/ptrace.h>


struct task_vfork {
	struct pt_regs ptregs;
};


#endif /* VFORK_RES_EXCHANGED_H_ */
