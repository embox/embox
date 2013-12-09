/*
	Describes lightweight thread structure.
*/

#ifndef _KERNEL_LWTHREAD_H_
#define _KERNEL_LWTHREAD_H_
struct runnable;

struct lwthread {
	struct runnable runnable;
}

extern struct lwthread * lwthread_create(void *(*run)(void *), void *arg);
extern void lwthread_free(struct lwthread *lwt);

#endif /* _KERNEL_LWTHREAD_H_ */
