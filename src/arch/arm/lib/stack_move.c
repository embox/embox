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

}

struct cpu_stack setjmp_stack_get(void *stack_base, jmp_buf env) {
	struct cpu_stack stack = {0};

	return stack;
}
