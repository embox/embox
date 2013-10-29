/**
 * @file
 *
 * @date Oct 24, 2013
 * @author: Anton Bondarev
 */

#ifndef IDESC_EVENT_H_
#define IDESC_EVENT_H_

#include <kernel/sched/wait_queue.h>

struct idesc_event {
	int flags;
	struct wait_link;
};

extern int idesc_event_lock(void);

extern int idesc_event_unlock(void);


#endif /* IDESC_EVENT_H_ */
