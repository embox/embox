/*
 * @file
 *
 * @date Oct 7, 2013
 * @author: Anton Bondarev
 */

#ifndef THREAD_LOCAL_H_
#define THREAD_LOCAL_H_

#include <module/embox/kernel/thread/thread_local.h>

extern void *thread_local_alloc(struct thread *t, size_t size);
#endif /* THREAD_LOCAL_H_ */
