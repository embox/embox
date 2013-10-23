/**
 * @file
 * @brief
 *
 * @date 22.10.2013
 * @author Vita Loginova
 */

#ifndef SCHED_WAIT_H_
#define SCHED_WAIT_H_

extern void sched_thread_notify(struct thread *thread, int result);
extern void sched_prepare_wait(void (*on_notified)(struct thread *, void *),
		void *data);

extern int sched_wait_locked(unsigned long timeout);

#endif /* SCHED_WAIT_H_ */
