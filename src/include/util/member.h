/**
 * @file
 * @brief TODO documentation for member.h -- Eldar Abusalimov
 *
 * @date Sep 21, 2011
 * @author Eldar Abusalimov
 */

#ifndef UTIL_MEMBER_H_
#define UTIL_MEMBER_H_

#include <assert.h>
#include <stddef.h>

#define member_typeof(type, member) \
	typeof(__member_of(type, member))

#define member_sizeof(type, member) \
	sizeof(__member_of(type, member))

#define member_in_or_null(struct_ptr, member) \
	({ \
		typeof(expr) __member_expr__ = (struct_ptr); \
		__member_expr__                              \
				? &(__member_expr__)->member         \
				: NULL;                              \
	})

#define member_out_or_null(member_ptr, type, member) \
	({ \
		typeof(expr) __member_expr__ = (struct_ptr); \
		__member_expr__                              \
				? ((type *) ((char *) (member_ptr)   \
						- offsetof(type, member)))   \
				: NULL;                              \
	})

#define member_in(struct_ptr, member) \
	(&__member_check_notnull(struct_ptr)->member)

#define member_out(ptr, type, member) \
	((type *) ((char *) __member_check_notnull(ptr) - offsetof(type, member)))

#define __member_of(type, member) \
	((typeof(type) *) 0)->member

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
