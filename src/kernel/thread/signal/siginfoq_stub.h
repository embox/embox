/**
 * @file
 * @brief Stubs for no signal support.
 *
 * @date 07.10.2013
 * @author Eldar Abusalimov
 */

#ifndef KERNEL_THREAD_SIGRT_H_
#define KERNEL_THREAD_SIGRT_H_

#include <errno.h>
#include <signal.h>

struct siginfoq { };  /* stub */

static inline struct siginfoq *siginfoq_init(struct siginfoq *infoq) {
	return infoq;
}

static inline int siginfoq_enqueue(struct siginfoq *infoq, int sig,
		const siginfo_t *info) {
	(void)infoq; (void)sig; (void)info;
	return -ENOSYS;
}

static inline int siginfoq_dequeue(struct siginfoq *infoq, int sig, siginfo_t *info) {
	(void)infoq; (void)sig; (void)info;
	return -ENOSYS;
}

#endif /* KERNEL_THREAD_SIGRT_H_ */
