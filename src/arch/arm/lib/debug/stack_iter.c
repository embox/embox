/**
 * @file stack_iter.c
 * @brief 
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version 0.1
 * @date 2016-02-29
 */

#include <stddef.h>

#include <hal/context.h>
#include <util/lang.h>
#include <util/math.h>

#include "stack_iter.h"

void stack_iter_context(stack_iter_t *f, struct context *ctx) {
}

void stack_iter_current(stack_iter_t *f) {
}

int stack_iter_next(stack_iter_t *f) {
	return 0;
}

void *stack_iter_get_fp(stack_iter_t *f) {
	return NULL;
}

void *stack_iter_get_retpc(stack_iter_t *f) {
	return NULL;
}
