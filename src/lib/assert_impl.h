/**
 * @file
 * @brief Assertion points implementation.
 *
 * @date Mar 25, 2011
 * @author Eldar Abusalimov
 */

#ifndef LIBC_ASSERT_IMPL_H_
#define LIBC_ASSERT_IMPL_H_

#include <util/location.h>

#ifdef NDEBUG

# define __assert(condition, expression_str) \
	do { } while (0)

#else

struct __assertion_point {
	struct location_func location;
	const char *expression;
};

extern void __attribute__ ((noreturn)) __assertion_handle_failure(
		const struct __assertion_point *point);

// TODO don't like it anymore. -- Eldar
# ifndef __ASSERT_HANDLE_NO_EXTERN_INLINE
extern inline
#  ifdef __GNUC_STDC_INLINE__
__attribute__((__gnu_inline__))
#  endif /* __GNUC_STDC_INLINE__ */
# else
/* Included from assert.c, emit global symbol for __assertion_handle. */
# endif /* __ASSERT_HANDLE_NO_EXTERN_INLINE */
void __assertion_handle(int pass, const struct __assertion_point *point) {
	if (!pass) {
		__assertion_handle_failure(point);
	}
}

# define __assertion_point__(expression_str) \
	__extension__ ({                                                \
		static const struct __assertion_point __assertion_point = { \
			.location = LOCATION_FUNC_INIT,                         \
			.expression = expression_str,                           \
		};                                                          \
		&__assertion_point;                                         \
	})

# define __assert(condition, expr_str) \
	(__builtin_constant_p(condition)                                     \
		? ((condition)                                                   \
			? (void) 0                                                   \
			: __assertion_handle_failure(__assertion_point__(expr_str))) \
		: __assertion_handle((int) (condition), __assertion_point__(expr_str)))

#endif /* NDEBUG */

#endif /* LIBC_ASSERT_IMPL_H_ */
