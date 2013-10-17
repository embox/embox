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

#define SIGRT_MIN     0
#define SIGRT_MAX     -1

struct sigrt_data { };  /* stub */

static inline struct sigrt_data * sigrt_data_init(
		struct sigrt_data *sigrt_data) {
	return sigrt_data;
}

static inline int sigrt_raise(struct sigrt_data *data, int sig,
		union sigval value) {
	return -ENOSYS;
}

static inline void sigrt_handle(struct sigrt_data *data,
		struct sigaction *sig_table) {
	/* no-op */
}

#endif /* KERNEL_THREAD_SIGRT_H_ */
