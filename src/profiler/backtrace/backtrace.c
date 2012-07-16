/**
 * @file
 * @brief TODO: brief
 *
 * @date 19 Apr, 2012
 * @author Bulychev Anton
 */

#include <module/embox/arch/frame.h>

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
