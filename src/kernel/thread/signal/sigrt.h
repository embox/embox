/**
 * @file
 * @brief RT signals.
 *
 * @date 07.10.2013
 * @author Eldar Abusalimov
 */

#ifndef KERNEL_THREAD_SIGRT_H_
#define KERNEL_THREAD_SIGRT_H_

#include <util/slist.h>

#define SIGRT_MIN     16
#define SIGRT_MAX     20

struct sigrt_data {
	struct slist pending;
};

#endif /* KERNEL_THREAD_SIGRT_H_ */
