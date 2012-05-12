/**
 * @file
 *
 * @brief
 *
 * @date 12.05.2012
 * @author Anton Bondarev
 */

#include <time.h>
#include <hal/clock.h>


clock_t clock(void) {
	return clock_sys_ticks();
}
