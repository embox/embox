/**
 * @file
 * @brief TODO: brief
 *
 * @date 19 Apr, 2012
 * @author Bulychev Anton
 */

#include <stdio.h>

struct frame {
	void *fp;
	void *pc;
};

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

void backtrace_fd(void) {
	struct frame f = get_current_frame();
	int depth = 0;

	printf("\nBacktrace:\n");
	printf("[%d] pc == 0x%p fp == %p\n", depth++, f.pc, f.fp);
	while (f.fp != NULL) {
		f = get_prev_frame(&f);
		if (f.pc == NULL) {
			break;
		}
		printf("[%d] pc == 0x%p fp == %p\n", depth++, f.pc, f.fp);
	}
	printf("\n");
}
