/**
 * @file
 *
 * @brief Timer facility and related utility functions.
 * @details
 * Use \ref itimer_alloc and \ref itimer_init to create a timer,
 * \ref itimer_read_ns and \ref itimer_read_timespec to query its remaining
 * time, and \ref itimer_free when timer is no longer needed.
 *
 * @date 15.05.2012
 * @author Anton Bondarev
 */

#ifndef KERNEL_TIME_TIMECOUNTER_H_
#define KERNEL_TIME_TIMECOUNTER_H_

#include <kernel/time/clock_source.h>
#include <kernel/time/ktime.h>

/**
 * A timer.
 *
 * @param start_value a \ref timespec representing timer's expiration time.
 * @param cs a pointer to a \ref clock_source to rely on when controlling
 *        timer's expiration.
 */
struct itimer {
	struct timespec start_value;
	struct clock_source *cs;
};

/**
 * Initializes an \ref itimer with the given clock source and expiration time.
 *
 * @param it a pointer to a memory block to store the new timer
 * @param clock_source a clock source to be used by the timer
 * @param start_tstamp a delay in nanoseconds after which the timer
 *        will expire. This delay is calculated relative to clock source
 *        current time.
 */
extern void itimer_init(struct itimer *it,
		struct clock_source *cs, time64_t start_tstamp);

/**
 * Returns a number of nanoseconds since the timer expired or before it will expire.
 *
 * @retval negative if the timer hasn't expired
 * @retval positive if the timer has expired
 */
extern time64_t itimer_read_ns(struct itimer *it);

/**
 * Populates given \ref timespec with a value representing the duration
 * of time between the start time of the given timer and the current
 * time of the timer's clock source. In other words, the duration
 * of time since the timer expired or the duration of time after which
 * the timer will expire.
 */
extern void itimer_read_timespec(struct itimer *it, struct timespec *ts);

/**
 * Allocates memory for a new timer in dedicated memory pool.
 */
extern struct itimer *itimer_alloc(void);

/**
 * Frees memory and returns it to pool.
 */
extern void itimer_free(struct itimer *it);

#endif /* KERNEL_TIME_TIMECOUNTER_H_ */
