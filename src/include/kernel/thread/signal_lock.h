/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    24.12.2013
 */

#ifndef THREAD_SIGNAL_LOCK_H_
#define THREAD_SIGNAL_LOCK_H_

static inline void threadsig_lock(void);
static inline void threadsig_unlock(void);

#include <module/embox/kernel/thread/signal/lock_api.h>

#endif /* THREAD_SIGNAL_LOCK_H_ */
