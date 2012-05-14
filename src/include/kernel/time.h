/**
 * @file
 *
 * @brief
 *
 * @date 14.05.2012
 * @author Anton Bondarev
 */

#ifndef KERNEL_TIME_H_
#define KERNEL_TIME_H_

struct ktimespec {
	long tv_sec;    /* seconds */
	long tv_nsec;   /* nanoseconds */
};


struct ktimeval {
	long tv_sec;    /* seconds */
	long tv_usec;   /* microseconds */
};

#endif /* KERNEL_TIME_H_ */
