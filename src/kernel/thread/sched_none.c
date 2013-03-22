/**
 * @file
 *
 * @date Mar 21, 2013
 * @author: Anton Bondarev
 */

struct sleepq;
struct event;

void sched_wake_all(struct sleepq *sq) {

}

int sched_sleep_ms(struct sleepq *sq, unsigned long timeout) {
	return 0;
}

int sched_sleep_locked_ms(struct sleepq *sq, unsigned long timeout) {
	return 0;
}


int sched_sleep_ns(struct sleepq *sq, unsigned long timeout) {
	return 0;
}


int sched_sleep_us(struct sleepq *sq, unsigned long timeout) {
	return 0;
}

int sched_sleep_locked_us(struct sleepq *sq, unsigned long timeout) {
	return 0;
}


int sched_sleep_locked_ns(struct sleepq *sq, unsigned long timeout) {
	return 0;
}
