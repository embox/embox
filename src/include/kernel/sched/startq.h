/**
 * @file
 * @brief TODO
 *
 * @date 13.09.2012
 * @author Anton Bulychev
 */

#ifndef KERNEL_SCHED_STARTQ_H_
#define KERNEL_SCHED_STARTQ_H_

#include <util/dlist.h>

#define STARTQ_DATA_NOT_IN_LIST 0
#define STARTQ_WAKE_SLEEPQ_ONE  1
#define STARTQ_WAKE_SLEEPQ_ALL  2
#define STARTQ_WAKE_THREAD      3

struct startq_data {
	struct dlist_head startq_link;
	int               in_list;
	void              (*dequeue)(struct startq_data *startq_data);
	/* This field is sleep result for thread and wake all for sleepq. */
	int               data;
};

extern void startq_init_thread(struct startq_data *startq_data);
extern void startq_init_sleepq(struct startq_data *startq_data);

#endif /* KERNEL_SCHED_STARTQ_H_ */
