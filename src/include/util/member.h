/**
 * @file
 * @brief struct/union member utilities: casts, sizeof/typeof, etc.
 *
 * @date 21.09.11
 * @author Eldar Abusalimov
 */

#ifndef UTIL_MEMBER_H_
#define UTIL_MEMBER_H_

#include <assert.h>
#include <stddef.h>

/**
 * A type describing a @a member inside a @a container type.
 *
 * Usage:
 * @code
 *
 *  // Suppose you have a compound type defined as above:
 *  struct foo {
 *  	...
 *  	struct bar bar; // Here is your member.
 *  	...
 *  };
 *
 *  // The following line defines a type 'bar_in_foo_t' which describes
 *  // member 'bar' inside the structure 'foo'.
 *  // Now one can cast between the member and its container using this type.
 *  typedef member_t(struct foo, bar) bar_in_foo_t;
 *
 * @endcode
 *
 * @param object_t
 *   Type of the container object: a struct or a union.
 * @param member_nm
 *   Name of the member inside the container.
 *   This can be a chain of names as well in case of nested containers,
 *   for example, @c bar[42].baz.
 */
#define member_t(object_t, member_nm) \
	union __attribute__ ((packed)) {                              \
		object_t object;                                      \
		struct __attribute__ ((packed)) {                     \
			char __offset[offsetof(object_t, member_nm)]; \
			member_typeof(object_t, member_nm) member;    \
		} /* unnamed */;                                      \
	}

/**
 * Retrieves a pointer to a member inside a given object.
 *
 * @param object_ptr
 *   The pointer to the object being inspected. Must not be null.
 * @param member_type
 *   A type describing the member (see #member_t()).
 * @return
 *   Pointer to the member within the object.
 */
#define member_of_object(object_ptr, member_type) \
	member_cast_in((member_type *) (object_ptr), member)

/**
 * A version of #member_of_object() which allows @a object_ptr to be null.
 *
 * @param object_ptr
 * @param member_type
 * @return
 *   Pointer to the member if @a object_ptr is a non-null pointer.
 * @retval NULL
 *   In case when the first argument is also @c NULL.
 */
#define member_of_object_or_null(object_ptr, member_type) \
	member_cast_in_or_null((member_type *) (object_ptr), member)

/**
 * Casts a pointer to a given member back to the object containing it.
 *
 * @param member_ptr
 *   The pointer to the member being casted. Must not be null.
 * @param member_type
 *   A type describing the member (see #member_t()).
 * @return
 *   Pointer to the object.
 */
#define member_to_object(member_ptr, member_type) \
	&(member_cast_out((member_ptr), member_type, member)->object)

/**
 * A version of #member_to_object() which allows @a member_ptr to be null.
 *
 * @param member_ptr
 * @param member_type
 * @return
 *   Pointer to the element if @a member_ptr is a non-null pointer.
 * @retval NULL
 *   In case when the first argument is also @c NULL.
 */
#define member_to_object_or_null(member_ptr, member_type) \
	((member_typeof(member_type, object) *) \
		member_cast_out_or_null((member_ptr), member_type, member))

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
	typeof(((typeof(type) *) 0x0)->member)

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
	sizeof(((typeof(type) *) 0x0)->member)

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
#define member_cast_in(struct_ptr, member) \
	(&__member_check_notnull(struct_ptr)->member)

/**
 * A version of #member_cast_in() which allows @a struct_ptr to be null.
 *
 * @param struct_ptr
 * @param member
 * @return
 *   Pointer to the member if @a struct_ptr is a non-null pointer.
 * @retval NULL
 *   In case when the first argument is also @c NULL.
 */
#define member_cast_in_or_null(struct_ptr, member) \
	({ \
		typeof(struct_ptr) __member_expr__ = (struct_ptr); \
		__member_expr__                                    \
			? &__member_expr__->member                 \
			: (typeof(&__member_expr__->member)) NULL; \
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
#define member_cast_out(member_ptr, type, member) \
	((type *) ((char *) __member_check_notnull(member_ptr) \
			- offsetof(type, member)))

/**
 * A version of #member_cast_out() which allows @a member_ptr to be null.
 *
 * @param member_ptr
 * @param type
 * @param member
 * @return
 *   @a type * pointer to the container if @a member_ptr is a non-null pointer.
 * @retval NULL
 *   In case when the first argument is also @c NULL.
 */
#define member_cast_out_or_null(member_ptr, type, member) \
	({                                                                \
		char *__member_expr__ = (char *) (member_ptr);            \
		(type *) (__member_expr__ ?                               \
			__member_expr__ - offsetof(type, member) : NULL); \
	})

#ifndef NDEBUG
# define __member_check_notnull(expr) \
	({                                             \
		typeof(expr) __member_expr__ = (expr); \
		assert(__member_expr__ != NULL);       \
		__member_expr__;                       \
	})
#else
# define __member_check_notnull(expr) (expr)
#endif /* NDEBUG */

#endif /* UTIL_MEMBER_H_ */
