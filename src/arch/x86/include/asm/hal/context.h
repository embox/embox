#ifndef HAL_CONTEXT_H_
# error "Do not include this file directly!"
#endif /* HAL_CONTEXT_H_ */

#include <setjmp.h>
#include <types.h>

struct context {
	jmp_buf buf;
};

