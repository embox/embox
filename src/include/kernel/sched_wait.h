/**
 * @file
 * @brief
 *
 * @date 22.10.2013
 * @author Vita Loginova
 */

#ifndef SCHED_WAIT_H_
#define SCHED_WAIT_H_

extern int sched_wait_init(void);
extern void sched_thread_notify(struct thread *thread, int result);
extern void sched_prepare_wait(void (*on_notified)(struct thread *, void *),
		void *data);
extern void sched_cleanup_wait(void);

extern int sched_wait(unsigned long timeout);
extern int sched_wait_locked(unsigned long timeout);

extern void sched_wait_run(void);

#endif /* SCHED_WAIT_H_ */
