/**
 * @file
 * @brief Stubs for standard signals.
 *
 * @date 07.10.2013
 * @author Eldar Abusalimov
 */

#ifndef KERNEL_THREAD_SIGSTD_STUB_H_
#define KERNEL_THREAD_SIGSTD_STUB_H_

#include <errno.h>
#include <signal.h>
#include <sys/cdefs.h>

struct sigstate { };  /* stub */

__BEGIN_DECLS

static inline struct sigstate *sigstate_init(
		struct sigstate *sigstate) {
	return sigstate;
}

static inline int sigstate_send(struct sigstate *data, int sig,
		const siginfo_t *info) {
	(void)data; (void)sig; (void)info;
	return -ENOSYS;
}

static inline int sigstate_receive(struct sigstate *data, siginfo_t *info) {
	(void)data; (void)info;
	return 0;
}

__END_DECLS

#endif /* KERNEL_THREAD_SIGSTD_STUB_H_ */
