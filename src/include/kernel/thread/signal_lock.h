/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    24.12.2013
 */

#ifndef THREAD_SIGNAL_LOCK_H_
#define THREAD_SIGNAL_LOCK_H_

#include <sys/cdefs.h>

__BEGIN_DECLS

extern void threadsig_lock(void);

extern void threadsig_unlock(void);

__END_DECLS

#endif /* THREAD_SIGNAL_LOCK_H_ */
