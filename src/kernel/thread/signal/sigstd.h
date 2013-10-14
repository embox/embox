/**
 * @file
 * @brief Std signals only.
 *
 * @date 07.10.2013
 * @author Eldar Abusalimov
 */

#ifndef KERNEL_THREAD_SIGSTD_H_
#define KERNEL_THREAD_SIGSTD_H_

#include <signal.h>

#define SIGSTD_MIN     1
#define SIGSTD_MAX     15

struct sigstd_data {
	sigset_t pending;
};

#endif /* KERNEL_THREAD_SIGSTD_H_ */
