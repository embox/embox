/*
 * @file
 *
 * @date Oct 7, 2013
 * @author: Anton Bondarev
 */

#ifndef THREAD_LOCAL_H_
#define THREAD_LOCAL_H_

#include <stddef.h>


struct thread;

#include <module/embox/kernel/thread/thread_local.h>

#define MODOPS_THREAD_KEY_QUANTITY OPTION_MODULE_GET( \
			embox__kernel__thread__thread_local, \
			NUMBER, thread_key_quantity)

typedef __thread_local_t thread_local_t;

extern int thread_local_alloc(struct thread *t, size_t size);
extern int thread_local_free(struct thread *t);

extern void *thread_local_get(struct thread *t, size_t idx);

extern int thread_local_set(struct thread *t, size_t idx, void *value);

#endif /* THREAD_LOCAL_H_ */
