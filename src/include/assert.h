/**
 * @file
 * @brief C standard library header.
 * @details
 *   Provides #assert() macro definition which is used in debugging purposes.
 *
 * @date 25.11.09
 * @author Eldar Abusalimov
 */

#ifndef ASSERT_H_
#define ASSERT_H_

#include __impl_x(lib/assert_impl.h)

/**
 * If @a condition expression evaluates to @c 0 (@c false), then
 * @link #panic() @endlink function is called with the message containing the
 * expression, source code filename, and line number as its argument.
 *
 * If the identifier NDEBUG ("no debug") is defined with
 * @code #define NDEBUG @endcode then the macro @c assert does nothing.
 *
 * @param condition
 *   The expression being asserted.
 */
#define assert(condition) \
	  __assert(condition, #condition)

#endif /* ASSERT_H_ */
