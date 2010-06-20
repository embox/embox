/**
 * @file
 * @brief Simple facility for producing unique symbols.
 * @details Guards are useful when defining multiple "anonymous" (named using
 * the same pattern) variables. In such cases guard prevents "variable
 * redefinition" compile-time error.
 *
 * @date 14.06.2010
 * @author Eldar Abusalimov
 */

#ifndef UTIL_GUARD_H_
#define UTIL_GUARD_H_

#include <impl/util/guard.h>

/**
 * Prepends guard to the specified symbol.
 *
 * @param s symbol (or macro which expand to symbol) to protect
 * @return symbol prefixed with the guard
 */
#define GUARD_PREFIX(s) __GUARD_PREFIX(s)

/**
 * Appends guard to the specified symbol.
 *
 * @param s symbol (or macro which expand to symbol) to protect
 * @return symbol suffixed with the guard
 */
#define GUARD_SUFFIX(s) __GUARD_SUFFIX(s)

#endif /* UTIL_GUARD_H_ */
