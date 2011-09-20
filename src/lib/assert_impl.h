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

#ifdef NDEBUG

# define __assert(condition, expr_str, message...) \
	do { } while (0)

#else

struct __assertion_point {
	struct location_func location;
	const char *expression;
};

extern void __attribute__ ((noreturn)) __assertion_handle_failure(
		const struct __assertion_point *point);

# define __assert(condition, expr_str, message...) \
	do { \
		if (!(condition)) {                                             \
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
