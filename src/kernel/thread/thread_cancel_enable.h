/**
 * @file
 *
 * @date Oct 15, 2013
 * @author: Anton Bondarev
 */

#ifndef THREAD_CANCEL_ENABLE_H_
#define THREAD_CANCEL_ENABLE_H_

#define CLEANUPS_QUANTITY \
	OPTION_MODULE_GET(embox__kernel__thread__thread_cancel_enable, \
			NUMBER, cleanups_quantity)

struct thread_cleanup {
	void (*routine)(void *);
	void *arg;
};

struct thread_cancel {
	unsigned int type;
	unsigned int state;
	unsigned int counter;
	struct thread_cleanup cleanups[CLEANUPS_QUANTITY];
};
typedef struct thread_cancel thread_cancel_t;


#endif /* THREAD_CANCEL_ENABLE_H_ */
