/**
 * @file
 * @brief TODO
 *
 * @date 13.09.2012
 * @author Anton Bulychev
 */

#ifndef KERNEL_THREAD_STARTQ_H_
#define KERNEL_THREAD_STARTQ_H_

#include <util/dlist.h>

#define STARTQ_DATA_NOT_IN_LIST 0
#define STARTQ_WAKE_SLEEPQ_ONE  1
#define STARTQ_WAKE_SLEEPQ_ALL  2
#define STARTQ_WAKE_THREAD      3

struct startq_data {
	struct dlist_head startq_link;
	int               info;
	int               sleep_res;      /* Used only for threads. */
};

static inline void startq_init(struct startq_data *startq_data) {
	dlist_head_init(&startq_data->startq_link);
	startq_data->info = STARTQ_DATA_NOT_IN_LIST;
}

#endif /* KERNEL_THREAD_STARTQ_H_ */
