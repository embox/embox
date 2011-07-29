/**
 * @file
 *
 * @brief interface to set periodical timers with milliseconds precision.
 *
 * @details
 *   for set timer use `init_timer' or `set_timer' functions.
 *   for emulate non-periodical behavior use close_timer function in the end of handler.
 *
 * @date 20.07.10
 * @author Fedor Burdun
 * @author Ilia Vaprol
 */

#ifndef KERNEL_TIMER_H_
#define KERNEL_TIMER_H_

#include <stdint.h>
#include <util/macro.h>

/**
 * timer types
 */
struct sys_tmr;
typedef struct sys_tmr sys_tmr_t;

/**
 * Type of timer's handler. Function that will be called by timers evrey n-th milliseconds.
 */
typedef void (*TIMER_FUNC)(sys_tmr_t *timer, void *param);

#include __module_headers(core/kernel/timer/timer_api)

/**
 * Set 'handle' timer for executing every 'ticks' ms.
 * Memory for set_tmr instance should be allocated before run init_timer.
 *
 * @param ptimer is poiter to preallocated sys_tmr_ptr.
 * @param ticks assignable time (quantity of milliseconds)
 * @param handler the function to be executed
 *
 * @return whether the timer is set
 * @retval 0 if the timer is set
 * @retval non-0 if the timer isn't set
 */
extern int init_timer(sys_tmr_t *ptimer, uint32_t ticks, TIMER_FUNC handle, void *param);

/**
 * Set 'handle' timer for executing every 'ticks' ms.
 * Memory for set_tmr instance will be allocated inside set_timer.
 *
 * @param ptimer is poiter to sys_tmr_ptr. If ptimer isn't null after call set_timer
 * it will be set to allocated sys_tmr_ptr.
 *
 * @param ticks assignable time (quantity of milliseconds)
 * @param handler the function to be executed
 *
 * @return whether the timer is set
 * @retval 0 if the timer is set
 * @retval non-0 if the timer isn't set
 */
extern int set_timer(sys_tmr_t **ptimer, uint32_t ticks, TIMER_FUNC handle, void *param);

/**
 * Shut down timer with system_tmr_t identity
 *
 * @param id timer identifier
 */
extern int close_timer(sys_tmr_t *ptimer);

#endif /* KERNEL_TIMER_H_ */
