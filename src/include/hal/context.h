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

#include <module/embox/arch/context.h>

#ifndef __ASSEMBLER__

#include <stdbool.h>

struct context;

#define CONTEXT_PRIVELEGED (1 << 0) /* TODO: ? */
#define CONTEXT_IRQDISABLE (1 << 1)

/**
 * Initialize new context.
 *
 * @param ctx Context to initialize
 * @param flags ORed flags to modify context features
 * @param routine_fn Context's IP (instruction pointer)
 * @param sp Context's SP (stack pointer)
 */
extern void context_init(struct context *ctx, unsigned int flags,
		void (*routine_fn)(void), void *sp);

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

#define CONTEXT_JMP_NEW_STACK(routine, sp) \
	struct context ctx##__LINE__, old_ctx##__LINE__; \
	context_init(&ctx##__LINE__, CONTEXT_PRIVELEGED, routine, sp); \
	context_switch(&old_ctx##__LINE__, &ctx##__LINE__)

#endif /* __ASSEMBLER__ */

#endif /* HAL_CONTEXT_H_ */
