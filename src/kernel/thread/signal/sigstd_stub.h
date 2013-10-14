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

#define SIGSTD_MIN     0
#define SIGSTD_MAX     -1

struct sigstd_data { };  /* stub */

static inline struct sigstd_data * sigstd_data_init(
		struct sigstd_data *sigstd_data) {
	return sigstd_data;
}

static inline int sigstd_raise(struct sigstd_data *data, int sig) {
	return -ENOSYS;
}

static inline void sigstd_handle(struct sigstd_data *data,
		void (**handlers)(int)) {
	/* no-op */
}

#endif /* KERNEL_THREAD_SIGSTD_STUB_H_ */
