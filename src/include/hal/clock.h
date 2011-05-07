/**
 * @file
 * @brief System timer unit interface.
 *
 * @date 25.11.09
 * @author Eldar Abusalimov
 */

#ifndef HAL_CLOCK_H_
#define HAL_CLOCK_H_

#include <asm/hal/clock.h>

#include <sys/types.h>

/**
 * Alias used to disable a clock when calling #clock_setup().
 */
#define CLOCK_SETUP_DISABLE 0

/**
 * Initializes hardware timer unit.
 *
 * @note Implementation have to disable all system timers.
 */
void clock_init(void);

/**
 * Configures the hardware clock.
 *
 * @param useconds positive value as timer period in microseconds,
 * or #CLOCK_SETUP_DISABLE to disable the clock
 */
void clock_setup(useconds_t useconds);

void clock_tick_handler(int irq_num, void *dev_id);

#endif /* HAL_CLOCK_H_ */
