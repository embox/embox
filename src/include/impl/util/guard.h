/**
 * @file
 * @brief TODO
 *
 * @date 14.06.2010
 * @author Eldar Abusalimov
 */

#ifndef UTIL_GUARD_H_
# error "Do not include this file directly, use <util/array.h> instead!"
#endif /* UTIL_GUARD_H_ */

#include <util/macro.h>

#define __GUARD_SYMBOL    MACRO_CONCAT(__line__, __LINE__)

#define __GUARD_PREFIX(s) MACRO_CONCAT(__GUARD_SYMBOL, s)
#define __GUARD_SUFFIX(s) MACRO_CONCAT(s, __GUARD_SYMBOL)
