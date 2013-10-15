/**
 * @file
 *
 * @date Oct 15, 2013
 * @author: Anton Bondarev
 */

#ifndef THREAD_CANCEL_H_
#define THREAD_CANCEL_H_

#include <module/embox/kernel/thread/thread_cancel.h>

extern int thread_cancel(struct thread *t);

extern int thread_cancel_set_state(int, int*);

extern int thread_cancel_set_type(int, int*);

extern int thread_cancel_cleanup_push(void (*)(void *), void *arg);

extern int thread_cancel_cleanup_pop(int);

extern int thread_cancel_init(struct thread *t);

#endif /* THREAD_CANCEL_H_ */
