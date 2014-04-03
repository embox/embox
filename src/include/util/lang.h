/**
 * @file
 * @brief Hides some deep compiler magic.
 * @details
 *   Most of utils from here should be used with care or avoided at all,
 *   because they rely on non-standard C extensions provided by GCC.
 *
 *   Please think twice before using these utils (and then think once again).
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
 * Surrounds an @a expr with @a pre_stmt and @a post_stmt statements.
 *
 * @return An expression of the same type as the given one.
 */
#define __lang_surround(expr, pre_stmt, post_stmt) \
	({ pre_stmt; __lang_eval_before(expr, post_stmt); })

/** @return An expression of the same type as the given one. */
#define __lang_eval_before(expr, post_stmt) \
	__lang_void_switch(expr,          \
		__lang_eval_void_before,    \
		__lang_eval_nonvoid_before, \
		post_stmt)

/** @return An expression of the same type as the given one. */
#define __lang_eval_after(expr, pre_stmt) \
	({ pre_stmt; (expr); })


/** The same as #__lang_surround, but does not return the result of @a expr. */
#define __lang_surround_void(expr, pre_stmt, post_stmt) \
	({ pre_stmt; __lang_eval_void_before(expr, post_stmt); })

/** The same as #__lang_eval_before, but returns nothing. */
#define __lang_eval_void_before(expr, post_stmt) \
	({ (void) (expr); post_stmt; (void) 0; })

/** The same as #__lang_eval_after, but returns nothing. */
#define __lang_eval_void_after(expr, pre_stmt) \
	({ pre_stmt; (void) (expr); })


/** The same as #__lang_surround for @a expr known to be non-void. */
#define __lang_surround_nonvoid(expr, pre_stmt, post_stmt) \
	({ pre_stmt; __lang_eval_nonvoid_before(expr, post_stmt); })

/** The same as #__lang_eval_before for @a expr known to be non-void. */
#define __lang_eval_nonvoid_before(expr, post_stmt) \
	({ typeof(expr) __lang_ret = (expr); post_stmt; __lang_ret; })

/** The same as #__lang_eval_after for @a expr known to be non-void. */
#define __lang_eval_nonvoid_after(expr, pre_stmt) \
	({ typeof(expr) __lang_ret; pre_stmt; __lang_ret = (expr); __lang_ret; })


/** Can be used to refer an extern variable without explicit declaration. */
#define __lang_extern_ref(type, name) \
	({ extern typeof(type) name; &name; })

#endif /* UTIL_LANG_H_ */
