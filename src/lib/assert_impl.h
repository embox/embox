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

#ifndef __ASSERT_HANDLE_NO_EXTERN_INLINE
extern inline
#else
/* Included from assert.c, emit global symbol for __assertion_handle. */
#endif
void __assertion_handle(int pass, const struct __assertion_point *point) {
	extern void __assertion_handle_failure(
			const struct __assertion_point *point);
	if (!pass) {
		__assertion_handle_failure(point);
	}
}

# define __assertion_point_ref(expression_str) \
	__extension__ ({                                                \
		static const struct __assertion_point __assertion_point = { \
			.location = LOCATION_FUNC_INIT,                         \
			.expression = expression_str,                           \
		};                                                          \
		&__assertion_point;                                         \
	})

# define __assert(condition, expr_str) \
	__builtin_constant_p(condition) && (condition) \
		? (void) 0 : __assertion_handle((int) (condition), \
				__assertion_point_ref(expr_str))

#endif /* NDEBUG */

#endif /* LIBC_ASSERT_IMPL_H_ */
