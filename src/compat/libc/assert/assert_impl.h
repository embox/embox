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
#include <linux/compiler.h>
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
# define __assert(condition, expr_str, ...) \
	do { } while ((typeof(condition)) 0)

#else

struct __assertion_point {
	struct location_func location;
	const char *expression;
};

#ifdef __cplusplus
extern "C" {
#endif

extern void __attribute__ ((noreturn)) __assertion_handle_failure(
		const struct __assertion_point *point);
extern char __assertion_message_buff[];
extern int sprintf(char *s, const char *format, ...);

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
# define __if_cplusplus(...) __VA_ARGS__
# define __if_not_cplusplus(...)
# define __assert_switch(cond, if_true, if_false) \
	((cond) ? (if_true) : (if_false))
#else
# define __if_cplusplus(...)
# define __if_not_cplusplus(...) __VA_ARGS__
# define __assert_switch __builtin_choose_expr
#endif

#define __assert(condition, expr_str, ...) \
	do { \
		if (!(likely(condition))) {                                     \
			static const struct __assertion_point __assertion_point = { \
				__if_not_cplusplus(.location   =) LOCATION_FUNC_INIT,   \
				__if_not_cplusplus(.expression =) expr_str,             \
			};                                                          \
			__assert_message_sprintf("" __VA_ARGS__ );                 \
			__assertion_handle_failure(&__assertion_point);             \
		}                                                               \
	} while(0)

#define __assert_message_sprintf(fmt, ...) \
	(void) __assert_switch(sizeof(fmt) != 1,             \
		sprintf(__assertion_message_buff,                \
			__assert_switch(sizeof(fmt) != 1, fmt, " "), \
                ## __VA_ARGS__), \
		(int) 0)

#endif /* NDEBUG */

#endif /* LIBC_ASSERT_IMPL_H_ */
