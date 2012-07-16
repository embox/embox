/**
 * @file
 * @brief TODO: brief
 *
 * @date 16 Jul, 2012
 * @author Bulychev Anton
 */

typedef struct pt_regs {
	uint32_t esp2, ss2;  // stack pointer just before entering the handler if there was no priority change. Are invalid otherwise, use esp, ss instead
	uint32_t gs, fs, es, ds;
	uint32_t edi, esi, ebp, cr2, ebx, edx, ecx, eax; /*pusha*/
	uint32_t trapno, err;              /*push by isr*/
	uint32_t eip, cs, eflags; /*Pushed by the processor automatically*/
	uint32_t esp, ss; /*Pushed by the processor automatically in case of priority level change. Are invalid otherwise (use esp2, ss2 instead) */
} pt_regs_t;


struct frame get_current_frame(void) {
	struct frame f;
	f.fp = __builtin_frame_address(0);
	f.pc = __builtin_return_address(0);
	return f;
}

struct frame get_prev_frame(struct frame *f) {
	struct frame prev_f;
	pt_regs_t *r;
	void **fp = f->fp;
	prev_f.fp = *fp;

	fp = prev_f.fp;
	if (*fp == NULL) {
		prev_f.pc = NULL;
	} else {
		prev_f.pc = *(fp+1);
	}

	if ((uint32_t)prev_f.pc == 0x10) {
		r = (pt_regs_t *) prev_f.fp;
		prev_f.fp = (void *) (r->ebp);
		prev_f.pc = (void *) (r->eip);
	}
	return prev_f;
}

