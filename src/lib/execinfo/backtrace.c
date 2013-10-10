/**
 * @file
 * @brief
 *
 * @date 19.04.12
 * @author Bulychev Anton
 * @author Ilia Vaprol
 */

#include <execinfo.h>
#include <module/embox/arch/stackframe.h>

extern void stack_iter_current(stack_iter_t *);
extern int stack_iter_next(stack_iter_t *);
//extern void *stack_iter_get_fp(stack_iter_t *);
extern void *stack_iter_get_retpc(stack_iter_t *);

int backtrace(void **buff, int size) {
	int i;
	stack_iter_t f;

	if (size == 0) {
		return 0;
	}

	stack_iter_current(&f);
	for (i = 0; i < size; ++i) {
		buff[i] = stack_iter_get_retpc(&f);
		if (!stack_iter_next(&f)) {
			++i;
			break;
		}
	}

	return i;
}
