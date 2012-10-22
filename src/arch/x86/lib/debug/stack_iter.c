/**
 * @file
 * @brief TODO: brief
 *
 * @date 16 Aug, 2012
 * @author Bulychev Anton
 */

#include <asm/traps.h>
#include "stack_iter.h"

void stack_iter_current(stack_iter_t *f) {
	f->fp = __builtin_frame_address(1);
	f->pc = __builtin_return_address(1);
}

int stack_iter_next(stack_iter_t *f) {
	void **p;

	if (TRAPS_TEXT_START <= (unsigned int) f->pc
	&& (unsigned int) f->pc < TRAPS_TEXT_END) {
		/* Through interruption */
		pt_regs_t *r = (pt_regs_t *) (f->fp + 8);
		f->fp = (void *) r->ebp;
		f->pc = (void *) r->eip;
	} else {
		p = f->fp; p = *p;
		if (*p == NULL) {
			f->fp = NULL;
			f->pc = NULL;
			return 0;
		} else {
			f->fp = p;
			f->pc = *(p+1);
		}
	}
	return 1;
}

void* stack_iter_get_fp(stack_iter_t *f) {
	return f->fp;
}

void* stack_iter_get_retpc(stack_iter_t *f) {
	return f->pc;
}

