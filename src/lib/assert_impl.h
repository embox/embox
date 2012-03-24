/**
 * @file
 * @brief Assertion points implementation.
 *
 * @date 25.03.11
 * @author Eldar Abusalimov
 */

#ifndef LIBC_ASSERT_IMPL_H_
#define LIBC_ASSERT_IMPL_H_

#include <util/location.h>
#include <compiler.h>

#ifdef NDEBUG

/* Do nothing.
 *
 * Implementation note: casting zero to typeof(condition) in the 'while' clause
 * is needed for two reasons:
 *   1. It forces compiler to check that the condition is a scalar value and
 *      thus can be used in 'if' when NDEBUG is off.
 *   2. It suppresses a warning about any variables that would be otherwise
 *      unused.
 */
# define __assert(condition, expr_str, message...) \
	do { } while ((typeof(condition)) 0)

#else

struct __assertion_point {
	struct location_func location;
	const char *expression;
};

extern void __attribute__ ((noreturn)) __assertion_handle_failure(
		const struct __assertion_point *point);

# define __assert(condition, expr_str, message...) \
	do { \
		if (!(likely(condition))) {                                             \
			extern char *__assertion_message_buff;                      \
			static const struct __assertion_point __assertion_point = { \
				.location = LOCATION_FUNC_INIT,                         \
				.expression = expr_str,                                 \
			};                                                          \
			extern int sprintf(char *s, const char *format, ...);       \
			sprintf(__assertion_message_buff, "" message);              \
			__assertion_handle_failure(&__assertion_point);             \
		}                                                               \
	} while(0)

#endif /* NDEBUG */

#endif /* LIBC_ASSERT_IMPL_H_ */
