/**
 * @file
 * @brief TODO:
 *
 * @date 25 Aug, 2012
 * @author Anton Bulychev
 */

#ifndef BACKTRACE_H_
#define BACKTRACE_H_

#include <module/embox/arch/stackframe.h>

void stack_iter_current(stack_iter_t *);
int stack_iter_next(stack_iter_t *);
void *stack_iter_get_fp(stack_iter_t *);
void *stack_iter_get_retpc(stack_iter_t *);

#endif /* BACKTRACE_H_ */
