/**
 * @file
 * @brief Hides some deep compiler magic.
 * @details
 *   Most of utils from here should be used with care or avoided at all,
 *   because they rely on non-standard C extensions provided by GCC.
 *
 * @date 16.03.13
 * @author Eldar Abusalimov
 */

#ifndef UTIL_LANG_H_
#define UTIL_LANG_H_

/** @return Whether the given @a expr has void type. */
#define __lang_typeof_void(expr) \
 	__builtin_types_compatible_p(typeof(expr), void)

/** @return An @a expr unchanged if it is not void, @a subst_with otherwise. */
#define __lang_subst_void(expr, subst_with) \
	__builtin_choose_expr(__lang_typeof_void(expr), subst_with, (expr))

/**
 * Depending on a type of an @a expr calls either @a on_void or @a on_rest
 * passing the original expression and varargs to it.
 *
 * @return A call to the second or the third argument.
 */
#define __lang_void_switch(expr, on_void, on_rest, ...) \
	__builtin_choose_expr(__lang_typeof_void(expr), \
			on_void(expr, ## __VA_ARGS__), \
			on_rest(__lang_subst_void(expr, 0), ## __VA_ARGS__))

/**
 * Surrounds an @a expr with @a stmt_before and @a stmt_after statements.
 *
 * @return An expression of the same type as the given one.
 */
#define __lang_surround(expr, stmt_before, stmt_after) \
	__lang_void_switch(expr, __lang_surround_void, __lang_surround_nonvoid, \
			stmt_before, stmt_after)

/** The same as #__lang_surround, but does not return the result of @a expr. */
#define __lang_surround_void(expr, stmt_before, stmt_after) \
	({                 \
		stmt_before;   \
		(void) (expr); \
		stmt_after;    \
		(void) 0;      \
	})

/** The same as #__lang_surround for @a expr known to be non-void. */
#define __lang_surround_nonvoid(expr, stmt_before, stmt_after) \
	({                           \
		typeof(expr) __lang_ret; \
		stmt_before;             \
		__lang_ret = (expr);     \
		stmt_after;              \
		__lang_ret;              \
	})

/** Can be used to refer an extern variable without explicit declaration. */
#define __lang_extern_ref(type, name) \
	({ extern typeof(type) name; &name; })

#endif /* UTIL_LANG_H_ */
