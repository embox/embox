/**
 * @file
 * @brief Provides @c assert macro as defined by C Standard Library.
 *
 * @date 25.11.09
 * @author Eldar Abusalimov
 */

#ifndef ASSERT_H_
#define ASSERT_H_

#include <kernel/panic.h>

#ifndef NDEBUG

/* we need to stringify condition before any expansion. */
# define __ASSERT(cond, cond_str) \
		do if (!(cond)) \
			panic("\nASSERTION FAILED: %s, at %s : %d\n", cond_str, __FILE__, \
					__LINE__); \
		while(0)
#else
# define __ASSERT(cond)   do ; while(0)
#endif /* NDEBUG */

/**
 * If expression evaluates to @c 0 (@c false), then @link panic() @endlink
 * function is called with the message containing the expression, sourcecode
 * filename, and line number as its argument.
 * If the identifier NDEBUG ("no debug") is defined with
 * @code #define NDEBUG @endcode then the macro @c assert does nothing.
 */
#define assert(cond)       __ASSERT(cond, #cond)

#endif /* ASSERT_H_ */
