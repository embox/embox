/**
 * @file
 * @brief
 *
 * @date 19.04.12
 * @author Bulychev Anton
 * @author Ilia Vaprol
 */

#include <stddef.h>

#include <execinfo.h>

#include <module/embox/arch/stackframe.h>
#include <hal/context.h>

extern void stack_iter_context(stack_iter_t *, struct context *);
extern void stack_iter_current(stack_iter_t *);
extern int stack_iter_next(stack_iter_t *);
extern void *stack_iter_get_retpc(stack_iter_t *);

int backtrace_context(void **buff, int size, struct context *ctx) {
	int i;
	stack_iter_t f;

	if (ctx)
		stack_iter_context(&f, ctx);
	else
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

int backtrace(void **buff, int size) {
	return backtrace_context(buff, size, NULL);
}
