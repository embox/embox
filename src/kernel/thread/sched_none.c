/**
 * @file
 *
 * @date Mar 21, 2013
 * @author: Anton Bondarev
 */


struct sleepq;
struct event;
struct thread;
typedef void (*notify_handler)(struct thread *, void *data);

void sched_wake_all(struct sleepq *sq) {

}

int sched_sleep(struct sleepq *sq, unsigned long timeout) {
	return 0;
}

int sched_sleep_locked(struct sleepq *sq, unsigned long timeout) {
	return 0;
}

int sched_tryrun(struct thread *thread) {
	return 0;
}

int sched_wait_locked(unsigned long timeout) {
	return 0;
}

void sched_thread_notify(struct thread *thread, int result) {
}

void sched_cleanup_wait(void) {
}

void sched_prepare_wait(notify_handler on_notified, void *data) {

}
