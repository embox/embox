#ifndef HAL_CONTEXT_H_
# error "Do not include this file directly!"
#endif /* HAL_CONTEXT_H_ */

#include <asm/ptrace.h>

struct context {
	struct pt_regs regs; // XXX at least for now... -- Eldar
};
