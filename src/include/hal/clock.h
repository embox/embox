/**
 * @file
 * @brief System timer unit interface.
 *
 * @date 25.11.2009
 * @author Eldar Abusalimov
 */

#ifndef HAL_CLOCK_H_
#define HAL_CLOCK_H_

#include <asm/hal/clock.h>

#include <sys/types.h>

/**
 * Alias used to disable a clock when calling @link clock_setup() @endlink .
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
 * or @link CLOCK_SETUP_DISABLE @endlink to disable the clock
 */
void clock_setup(useconds_t useconds);

#endif /* HAL_CLOCK_H_ */
