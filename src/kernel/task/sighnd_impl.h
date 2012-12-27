/**
 * @file
 *
 * @brief
 *
 * @date 27.12.12
 * @author Alexander Kalmuk
 */

#ifndef SIGNAL_IMPL_H_
#define SIGNAL_IMPL_H_

#include <kernel/task.h>

static inline void __sighnd_default(int sig) {
	task_exit(NULL);
}

static inline void __sighnd_ignore(int sig) { /* do nothing */}

#endif /* SIGNAL_IMPL_H_ */
