/*
 * @brief Kernel time
 *
 * @date 13.06.2012
 * @author Alexander Kalmuk
 */

#ifndef KERNEL_TIME_KTIME_H_
#define KERNEL_TIME_KTIME_H_

struct timespec {
	long sec;    /* seconds */
	long nsec;   /* nanoseconds */
};

struct timeval {
	long sec;    /* seconds */
	long usec;   /* microseconds */
};

struct timehw {
	long events;
	long cycles;
};

#endif /* KERNEL_TIME_KTIME_H_ */
