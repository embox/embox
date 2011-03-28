/**
 * @file
 * @brief TODO
 *
 * @date Mar 25, 2011
 * @author Eldar Abusalimov
 */

#include "api_impl.h"

// XXX initial value to prevent early scheduling from irq_leave.
__critical_t __critical_count = __CRITICAL_COUNT(__CRITICAL_PREEMPT);
