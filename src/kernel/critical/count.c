/**
 * @file
 * @brief TODO
 *
 * @date 25.03.11
 * @author Eldar Abusalimov
 */

#include "api_impl.h"

// XXX initial value to prevent early scheduling from irq_leave.
__critical_t __critical_count = 0;

