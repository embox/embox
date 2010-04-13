/**
 * @file
 * @brief Defines structure representing architecture-dependent execution
 * context and provides methods to deal with it.
 *
 * @date 12.04.2010
 * @author Eldar Abusalimov
 */

#ifndef HAL_CONTEXT_H_
#define HAL_CONTEXT_H_

#include <stdbool.h>

#include <asm/hal/context.h>

struct context;

void context_init(struct context *ctx, bool privileged);

void context_set_stack(struct context *ctx, void *sp);
void context_set_entry(struct context *ctx, void *pc);

void context_switch(struct context *prev, struct context *next);

#endif /* HAL_CONTEXT_H_ */
