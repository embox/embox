/**
 * @file
 * @brief TODO: brief
 *
 * @date 16 Aug, 2012
 * @author Bulychev Anton
 */

#include <stddef.h>

#include <asm/traps.h>
#include <hal/context.h>
#include <util/lang.h>
#include <util/math.h>

#include "stack_iter.h"

static inline int is_traps_pc(void *pc) {
	return check_range((char *) pc,
		__lang_extern_ref(char, _traps_text_start),
		__lang_extern_ref(char, _traps_text_end));
}

void stack_iter_context(stack_iter_t *f, struct context *ctx) {
	f->fp = (void *) ctx->ebp;
	f->pc = (void *) ctx->eip;
}

void stack_iter_current(stack_iter_t *f) {
	f->fp = __builtin_frame_address(1);
	f->pc = __builtin_return_address(1);
}

int stack_iter_next(stack_iter_t *f) {
	if (is_traps_pc(f->pc)) {
		/* Through interruption */
		pt_regs_t *r = (pt_regs_t *) (f->fp + 8);
		f->fp = (void *) r->ebp;
		f->pc = (void *) r->eip;
	} else {
		void **p = f->fp; p = *p;

		f->fp = p;
		f->pc = *(p+1);

		if ((*p == NULL && !is_traps_pc(f->pc)) || !f->pc) {
			f->fp = NULL;
			f->pc = NULL;
			return 0;
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
