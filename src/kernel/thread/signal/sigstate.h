/**
 * @file
 * @brief Std signals only.
 *
 * @date 07.10.2013
 * @author Eldar Abusalimov
 */

#ifndef KERNEL_THREAD_SIGSTD_H_
#define KERNEL_THREAD_SIGSTD_H_

#include <module/embox/kernel/thread/signal/siginfoq_api.h>

#include <signal.h>

#include <lib/libds/dlist.h>
#include <sys/cdefs.h>

struct siginfoq;

struct sigstate {
	sigset_t        pending;
	struct siginfoq infoq;
};

__BEGIN_DECLS

extern struct siginfoq *siginfoq_init(struct siginfoq *);

extern int siginfoq_enqueue(struct siginfoq *, int, const siginfo_t *);
extern int siginfoq_dequeue(struct siginfoq *, int, siginfo_t *);

__END_DECLS

#endif /* KERNEL_THREAD_SIGSTD_H_ */
