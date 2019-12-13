/**
 *
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

#if defined __cplusplus
# define __ASSERT_VOID_CAST static_cast<void>
#else
# define __ASSERT_VOID_CAST (void)
#endif

/* Do nothing.
 *
 * Older implementation just used "typeof(condition)" for a following reasons:
 *   1. It forces compiler to check that the condition is a scalar value and
 *      thus can be used in 'if' when NDEBUG is off.
 *   2. It suppresses a warning about any variables that would be otherwise
 *      unused.
 *
 * However, it turned out that typeof() couldn't be applied to bit fields (which we
 * don't use in Embox, but it's being used in some third-party code), so it's not an
 * accurate check for scalar value. "typeof(0 || (condition))" works better for it.
 */

/* Uncomment this part to suppress checks mentioned in the comment above
#define __assert(condition, expr_str, ...) \
	do { } while ((__typeof__(0 || (condition))) 0)
*/

#define __assert(...)

#else

#define ASSERT_MESSAGE_BUFF_SZ 128

struct __assertion_point {
	struct location_func location;
	const char *expression;
};

#ifdef __cplusplus
extern "C" {
#endif

extern void _NORETURN __assertion_handle_failure(const struct __assertion_point *point);
extern char __assertion_message_buff[ASSERT_MESSAGE_BUFF_SZ];
extern int snprintf(char *s, size_t size, const char *format, ...);

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
		snprintf(__assertion_message_buff,                \
				sizeof(__assertion_message_buff),        \
			__assert_switch(sizeof(fmt) != 1, fmt, " "), \
                ## __VA_ARGS__), \
		(int) 0)

#endif /* NDEBUG */

#endif /* LIBC_ASSERT_IMPL_H_ */
