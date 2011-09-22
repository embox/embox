/**
 * @file
 * @brief struct/union member utilities: casts, sizeof/typeof, etc.
 *
 * @date Sep 21, 2011
 * @author Eldar Abusalimov
 */

#ifndef UTIL_MEMBER_H_
#define UTIL_MEMBER_H_

#include <assert.h>
#include <stddef.h>

/**
 * Gets a type of @a member within the specified one.
 *
 * @param type
 *   The type of the container (or an expression of such type).
 * @param member
 *   The name of the member within the structure/union.
 * @return
 *   Result of @c typeof applied to the member.
 */
#define member_typeof(type, member) \
	typeof(__member_of(type, member))

/**
 * Gets a size of @a member within the specified @a type.
 *
 * @param type
 *   The type of the container (or an expression of such type).
 * @param member
 *   The name of the member within the structure/union.
 * @return
 *   Result of @c sizeof applied to the member.
 */
#define member_sizeof(type, member) \
	sizeof(__member_of(type, member))

#define __member_of(type, member) \
	((typeof(type) *) 0x0)->member

/**
 * Gets a pointer to @a member of the specified expression.
 *
 * @param struct_ptr
 *   The pointer to the container. Must not be null.
 * @param member
 *   The name of the member within the structure/union.
 * @return
 *   Pointer to the member.
 */
#define member_in(struct_ptr, member) \
	(&__member_check_notnull(struct_ptr)->member)

/**
 * A version of #member_in() which allows @a struct_ptr to be null.
 *
 * @return
 *   Pointer to the member if @a struct_ptr is a non-null pointer.
 * @retval NULL
 *   In case when the first argument is also @c NULL.
 */
#define member_in_or_null(struct_ptr, member) \
	({ \
		typeof(struct_ptr) __member_expr__ = (struct_ptr); \
		__member_expr__                                    \
				? &__member_expr__->member                 \
				: NULL;                                    \
	})

/**
 * Casts a member out to the containing structure/union.
 *
 * @param member_ptr
 *   The pointer to the member being casted. Must not be null.
 * @param type
 *   The type of the container.
 * @param member
 *   The name of the member within the structure/union.
 * @return
 *   @a type * pointer to the container.
 */
#define member_out(member_ptr, type, member) \
	((type *) ((char *) __member_check_notnull(member_ptr) \
			- offsetof(type, member)))

/**
 * A version of #member_out() which allows @a member_ptr to be null.
 *
 * @return
 *   @a type * pointer to the container if @a member_ptr is a non-null pointer.
 * @retval NULL
 *   In case when the first argument is also @c NULL.
 */
#define member_out_or_null(member_ptr, type, member) \
	({ \
		char *__member_expr__ = (char *) (member_ptr); \
		__member_expr__                                \
				? (type *) (__member_expr__            \
						- offsetof(type, member))      \
				: NULL;                                \
	})

#ifdef NDEBUG
# define __member_check_notnull(expr) \
	({ \
		typeof(expr) __member_expr__ = (expr); \
		assert(__member_expr__ != NULL);       \
		__member_expr__;                       \
	})
#else
# define __member_check_notnull(expr) \
	(expr)
#endif /* NDEBUG */

#endif /* UTIL_MEMBER_H_ */
