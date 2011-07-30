/**
 * @file
 * @brief   Defines API and data structure to deal with thread cpu_context.
 *
 * @details Defines structure representing architecture-dependent execution
 *          context and provides methods to deal with it.
 *          This file should include to work with thread's context instead
 *          'asm/...'
 *
 * @date 12.04.10
 * @author Eldar Abusalimov
 */

#ifndef HAL_CONTEXT_H_
#define HAL_CONTEXT_H_

#include <types.h>
#include <asm/hal/context.h>

#ifndef __ASSEMBLER__

struct context;

/**
 * Declare type for thread entry
 */
typedef void (*thread_routine_t)(void);

extern void context_init(struct context *ctx, bool privileged);

extern void context_set_stack(struct context *ctx, void *sp);

extern void context_set_entry(struct context *ctx, thread_routine_t routine);


/**
 * Changes current thread context. Save previous thread context and load
 * wished active thread context.
 * This function should call from c-code, because it
 * use stack frame in some CPU architectures.
 *
 * @param prev - pointer to current active thread structure
 * @param next - pointer to wished active thread structure
 */
extern void context_switch(struct context *prev, struct context *next);
#endif /* __ASSEMBLER__ */

#endif /* HAL_CONTEXT_H_ */
