/**
 * @file
 *
 * @date Jul 31, 2013
 * @author: Anton Bondarev
 */

#ifndef SCHED_RUNNING_TIME_H_
#define SCHED_RUNNING_TIME_H_

#include <sys/types.h>

struct sched_timing {
	clock_t            running_time;  /**< Running time of thread in clocks. */
	clock_t            last_sync;     /**< Last recalculation of running time.*/
};

#endif /* SCHED_RUNNING_TIME_H_ */
