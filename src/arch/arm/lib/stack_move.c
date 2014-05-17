/**
 * @file
 *
 * @data May 7, 2014
 * @author Anton Bondarev
 */

#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>

void setjmp_stack_move(ptrdiff_t offset, jmp_buf env) {
	env[SETJMP_EFP_INDEX] += offset;
	env[SETJMP_ESP_INDEX] += offset;

	*(uint32_t *)env[SETJMP_EFP_INDEX] += offset;
}

struct cpu_stack setjmp_stack_get(void *stack_base, jmp_buf env) {
	struct cpu_stack stack = {0};

	stack.ebp = (ptrdiff_t)stack_base - env[SETJMP_EFP_INDEX];
	stack.esp = (ptrdiff_t)stack_base - env[SETJMP_ESP_INDEX];

	return stack;
}
