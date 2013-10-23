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

struct sigstd_data {
	sigset_t pending;
};

#endif /* KERNEL_THREAD_SIGSTD_H_ */
