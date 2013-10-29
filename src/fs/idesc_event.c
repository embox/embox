/**
 * @file
 *
 * @date Oct 24, 2013
 * @author: Anton Bondarev
 */
#include <errno.h>
#include <kernel/sched/wait_queue.h>

int idesc_event_lock(void) {
	return -ENOSUPP;
}
int idesc_event_unlock(void) {
	return -ENOSUPP;
}
