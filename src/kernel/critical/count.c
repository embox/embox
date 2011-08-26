/**
 * @file
 * @brief TODO
 *
 * @date 25.03.11
 * @author Eldar Abusalimov
 */

#include <kernel/critical/api.h>

// XXX initial value.
__critical_t __critical_count = __CRITICAL_COUNT(CRITICAL_SCHED_LOCK);

struct critical_dispatcher *__critical_dispatch_queue;
