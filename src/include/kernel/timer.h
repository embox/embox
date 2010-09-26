/**
 * @file
 *
 * @date 02.09.2009
 * @author Andrey Baboshin
 */
#ifndef _TIMERS_H_
#define _TIMERS_H_

#include <types.h>

#include <kernel/irq.h>

#ifndef __ASSEMBLER__

typedef void (*TIMER_FUNC)(uint32_t id);

/**
 * Initialization of the timers subsystem.
 */
static int timer_init(void);

/**
 * Set 'handle' timer with 'id' identity for executing every 'ticks' ms.
 *
 * @param id timer identifier
 * @param ticks assignable time
 * @param handle the function to be executed
 *
 * @return whether the timer is set
 * @retval 1 if the timer is set
 * @retval 0 if the timer isn't set
 */
int set_timer(uint32_t id, uint32_t ticks, TIMER_FUNC handle);

/**
 * Shut down timer with 'id' identity
 *
 * @param id timer identifier
 */
void close_timer(uint32_t id);

/**
 * Save timers context. Now saving only one context.
 */
int timers_context_save(void);

/**
 * Restore context by it number.
 */
int timers_context_restore(int context_number);

/**
 * Shutdown timers subsystem.
 */
void timers_off(void);

uint32_t cnt_system_time(void);

#endif /*__ASSEMBLER__*/
#endif /*_TIMERS_H_*/
