/**
 * @file
 * @brief Manages active thread on CPU core
 *
 * @details This interface was extracted for optimization on different CPU and
 *      work in SMP mode.
 *      There are several principles working with current thread handler.
 *      Simplest is hold the current thread pointer in a special variable. If
 *      system run in SMP mode you reserve variable for each core using 'percpu'
 *      macro for this deal.
 *      Second work on RISC processors where is enough registers for this deal.
 *      In SMP mode it works correctly due to presence the registers in each CPU
 *      core.
 *      Third method works affectively for kernel threads which has fixed size
 *      stack. If stack is aligned by 'suit' value and thread structure is
 *      placed on own stack (top or bottom) it make available to get the thread
 *      handler through current stack pointer register. It is available for each
 *      processors. So if you get stack pointer and align it by stack size you
 *      can get thread handler.
 *
 * @date 13.02.12
 * @author Anton Bulychev
 */

#ifndef KERNEL_SCHEDEE_CURRENT_H_
#define KERNEL_SCHEDEE_CURRENT_H_

#include <module/embox/kernel/schedee/current/api.h>

#define schedee_get_current() \
	__schedee_get_current()

#define schedee_set_current(schedee) \
	__schedee_set_current(schedee)

#endif /* KERNEL_SCHEDEE_CURRENT_H_ */
