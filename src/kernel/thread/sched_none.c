/**
 * @file
 *
 * @date Mar 21, 2013
 * @author: Anton Bondarev
 */

struct sleepq;
struct event;
struct thread;

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
