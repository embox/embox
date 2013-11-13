/**
	Interface for thread entities, used by scheduler
*/
#ifndef _KERNEL_PLANNBLE_H_
#define _KERNEL_PLANNBLE_H_

#include <kernel/light_thread/light_thread.h>


struct plannable {
	int                *policy;       /**< Scheduling policy pointer*/
	struct sched_attr  *sched_attr;   /**< Scheduler-private data pointer */
}

/*
	Use this to create new plannable!
	Do NOT just allocate memory!
*/
extern struct plannable * plannable_create_thread(unsigned int flags,
		void *(*run)(void *), void *arg);
extern struct plannable * plannable_create_lthread(void *(*run)(void *),
		void *arg);

#endif /* _KERNEL_PLANNBLE_H_ */
