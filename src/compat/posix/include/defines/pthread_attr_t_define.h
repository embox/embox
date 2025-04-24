/**
 * @file
 * @brief
 *
 * @date   24.04.2025
 * @author Anton Bondarev
 */

#ifndef DEFINES_PTHREAD_ATTR_T_DEFINE_H_
#define DEFINES_PTHREAD_ATTR_T_DEFINE_H_

/*The following types shall be defined as described in <sys/types.h> :
 * ...
 */
#include <sys/types.h>

/* Inclusion of the <sched.h> header may make visible all symbols from
 * the <time.h> header.
 */
#include <sched.h>


typedef struct pthread_attr {
	uint32_t flags; /* scope, inherit, detachstate */
	void *stack;
	uint32_t stack_size;
	int policy;
	struct sched_param sched_param;
} pthread_attr_t;

#endif /* DEFINES_PTHREAD_ATTR_T_DEFINE_H_ */
