/**
 * @file clock.h
 *
 * @brief System timer unit interface.
 *
 * @date 25.11.2009
 * @author Eldar Abusalimov
 */

#ifndef HAL_CLOCK_H_
#define HAL_CLOCK_H_

#include <sys/types.h>

#include <asm/haldefs.h>

/**
 * Total amount of system timers.
 *
 * @note Implementation should provide @link __CLOCK_NRS_TOTAL @endlink
 * definition indicating positive constant.
 */
#define CLOCK_NRS_TOTAL __CLOCK_NRS_TOTAL

/**
 * Checks if the specified clock_nr is less then
 * @link CLOCK_NRS_TOTAL @endlink value.
 */
#define clock_nr_valid(clock_nr) \
	((clock_nr_t) clock_nr < (clock_nr_t) CLOCK_NRS_TOTAL)

/**
 * Alias used to disable a clock when calling @link clock_setup() @endlink .
 */
#define CLOCK_SETUP_DISABLE 0

/**
 * Type representing clock number.
 *
 * @note Implementation should provide @link __clock_nr_t @endlink type
 * indicating unsigned (it is essential!) integer suitable to hold up to
 * @link CLOCK_NRS_TOTAL @endlink values.
 */
typedef __clock_nr_t clock_nr_t;

/**
 * Initializes hardware timer unit.
 *
 * @note Implementation have to disable all system timers.
 */
void clock_init();

/**
 * Configures a clock with the specified number.
 *
 * @param clock_nr the clock to setup
 * @param useconds positive value as timer period in microseconds,
 * or @link CLOCK_SETUP_DISABLE @endlink to disable the clock
 */
void clock_setup(clock_nr_t clock_nr, useconds_t useconds);

#endif /* HAL_CLOCK_H_ */
