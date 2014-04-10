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
 *  	int bar; // Here is your member.
 *  	...
 *  };
 *
 *  // The following line defines a type 'foo_bar' which describes
 *  // member 'bar' inside the structure 'foo'.
 *  // Now one can cast between the member and its container using this type.
 *  typedef member_t(struct foo, bar) foo_bar;
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


/** &foo --> &foo.bar;  @a struct_ptr must not be null; @see #member_t() */
#define member_of_object(struct_ptr, member_type) \
	member_cast_in(member_cast_out(struct_ptr, \
			member_type, object), member)

/** &foo --> &foo.bar; NULL --> NULL;  @see #member_t() */
#define member_of_object_or_null(struct_ptr, member_type) \
	member_cast_in_or_null(member_cast_out_or_null(struct_ptr, \
			member_type, object), member)


/** &foo.bar --> &foo;  @a member_ptr must not be null; @see #member_t() */
#define member_to_object(member_ptr, member_type) \
	member_cast_in(member_cast_out(member_ptr, \
			member_type, member), object)

/** &foo.bar --> &foo; NULL --> NULL;  @see #member_t() */
#define member_to_object_or_null(member_ptr, member_type) \
	member_cast_in_or_null(member_cast_out_or_null(member_ptr, \
			member_type, member), object)


/** typeof(foo.bar);  @a type is (an expr of) a struct or a union */
#define member_typeof(type, member_nm) \
	typeof(((typeof(type) *) 0x0)->member_nm)

/** sizeof(foo.bar);  @a type is (an expr of) a struct or a union */
#define member_sizeof(type, member_nm) \
	sizeof(((typeof(type) *) 0x0)->member_nm)


/** &foo --> &foo.bar;  @a struct_ptr must not be null */
#define mcast_in(struct_ptr, member) \
	member_cast_in(struct_ptr, member)
#define member_cast_in(struct_ptr, member) \
	(&__member_check_notnull(struct_ptr)->member)

/** &foo --> &foo.bar; NULL --> NULL; */
#define mcast_in_or_null(struct_ptr, member) \
	member_cast_in_or_null(struct_ptr, member)
#define member_cast_in_or_null(struct_ptr, member) \
	({ \
		typeof(struct_ptr) __member_expr__ = (struct_ptr); \
		__member_expr__                                    \
			? &__member_expr__->member                 \
			: (typeof(&__member_expr__->member)) NULL; \
	})


/** &foo.bar --> &foo;  @a member_ptr must not be null */
#define mcast_out(member_ptr, type, member) \
	member_cast_out(member_ptr, type, member)
#define member_cast_out(member_ptr, type, member) \
	((type *) ((char *) __member_check_notnull(member_ptr) \
			- offsetof(type, member)))

/** &foo.bar --> &foo; NULL --> NULL; */
#define mcast_out_or_null(member_ptr, type, member) \
	member_cast_out_or_null(member_ptr, type, member)
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
