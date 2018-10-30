/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    29.05.2014
 */

#include <util/dlist.h>
#include <kernel/time/timer.h>

#include <kernel/thread/thread_wait.h>

void thread_wait_init(struct thread_wait *tw) {
	dlist_init(&tw->thread_waitq_list);
}

void thread_wait_add(struct thread_wait *tw, struct sys_timer *tmr) {
	dlist_add_next(&tw->thread_waitq_list, dlist_init(&tmr->st_wait_link));
}

void thread_wait_del(struct thread_wait *tw, struct sys_timer *tmr) {
	dlist_del(&tmr->st_wait_link);
}

void thread_wait_deinit(struct thread_wait *tw) {
	struct sys_timer *tmr;

	dlist_foreach_entry_safe(tmr, &tw->thread_waitq_list, st_wait_link) {
		timer_close(tmr);
	}
	dlist_init(&tw->thread_waitq_list);
}
