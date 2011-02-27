/**
 * @file
 * @brief Implements distributed arrays backed by linker section SORT facility.
 *
 * @date 13.06.2010
 * @author Eldar Abusalimov
 */

#ifndef UTIL_ARRAY_H_
# error "Do not include this file directly, use <util/array.h> instead!"
#endif /* UTIL_ARRAY_H_ */

#include <stddef.h>

#include <util/macro.h>
#include <util/guard.h>

/*
 * For each diffuse array three sections are created. These sections are then
 * placed together successively each after each.
 */

/* The symbol for the array name is placed in head section. */
#define __ARRAY_DIFFUSE_SECTION_HEAD(s_array) \
		__ARRAY_DIFFUSE_SECTION(s_array, 0, head)

/* Body hosts the array elements (excepting terminator, if any). */
#define __ARRAY_DIFFUSE_SECTION_BODY(s_array) \
		__ARRAY_DIFFUSE_SECTION(s_array, 1, body)

/* Contains the array end marker and terminator element (if any). */
#define __ARRAY_DIFFUSE_SECTION_TAIL(s_array) \
		__ARRAY_DIFFUSE_SECTION(s_array, 9, tail)

/*
 * All array_diffuse-related sections are linked using something
 * like *(SORT(.array_diffuse.*.rodata)). SORT guarantees that the wildcard is
 * expanded in the lexicographical order.
 */

/* Order argument is used to control the relative placement of sections within
 * a particular array.
 * Tag is just appended to the name of section (in debugging purposes). */
#define __ARRAY_DIFFUSE_SECTION(s_array, order, tag) \
	".array_diffuse." MACRO_STRING(s_array##_##order##_##tag) ".rodata"

/* Every array entry, group of entries or marker symbol is backed by an
 * individual array (empty for markers) defined as follows
 *  __extension__ bypasses compiler warnings about empty arrays*/
#define __ARRAY_DIFFUSE_ENTRY_DEF(type, s_entry, _section) \
	__extension__ type s_entry[] __attribute__ ((used, section(_section)))

/* Diffuse arrays implementation-private entries are named as follows
 * to prevent namespace pollution. */
#define __ARRAY_DIFFUSE_PRIVATE(s_array, s_tag) \
	__array_diffuse_private__##s_array##__##s_tag

#define __ARRAY_DIFFUSE_PRIVATE_DEF(type, s_array, s_tag, section) \
	__ARRAY_DIFFUSE_ENTRY_DEF(type, \
		__ARRAY_DIFFUSE_PRIVATE(s_array, s_tag), section)

/* Here goes API implementation. */

#define __ARRAY_DIFFUSE_DEF(element_type, array_name) \
	__ARRAY_DIFFUSE_DEF_TERMINATED(element_type, array_name, /* empty */)

#define __ARRAY_DIFFUSE_DEF_TERMINATED(element_type, array_name, terminator) \
	__ARRAY_DIFFUSE_ENTRY_DEF(element_type, array_name, \
		__ARRAY_DIFFUSE_SECTION_HEAD(array_name)) = { /* empty */ }; \
	__ARRAY_DIFFUSE_PRIVATE_DEF(element_type, array_name, tail, \
		__ARRAY_DIFFUSE_SECTION_TAIL(array_name)) = { terminator }

#define __ARRAY_DIFFUSE_DEF_STATIC(element_type, array_name) \
	__ARRAY_DIFFUSE_DEF(static element_type, array_name)

#define __ARRAY_DIFFUSE_DEF_TERMINATED_STATIC(element_type, name, terminator) \
	__ARRAY_DIFFUSE_DEF_TERMINATED(static element_type, name, terminator)

#define __ARRAY_DIFFUSE_ADD(array_name, ...) \
	__ARRAY_DIFFUSE_ADD_NAMED(array_name, \
		GUARD_SUFFIX(__ARRAY_DIFFUSE_PRIVATE(array_name, entry)), \
		__VA_ARGS__)

#define __ARRAY_DIFFUSE_ADD_NAMED(array_name, ptr_name, ...) \
	__ARRAY_DIFFUSE_ENTRY_DEF(static typeof(array_name[0]), ptr_name, \
		__ARRAY_DIFFUSE_SECTION_BODY(array_name)) = { __VA_ARGS__ }

#define __ARRAY_DIFFUSE_SIZE(array_name) __extension__ ({ \
		extern typeof(array_name) __ARRAY_DIFFUSE_PRIVATE(array_name, tail); \
		(size_t) (__ARRAY_DIFFUSE_PRIVATE(array_name, tail) - array_name); \
	})

#define __ARRAY_SIZE(array_name) \
	(sizeof(array_name) / sizeof((array_name)[0]))

#define __array_static_foreach_ptr(element_ptr, array) \
		__array_foreach_ptr(element_ptr, array, ARRAY_SIZE(array))

#define __array_nullterm_foreach_ptr(element_ptr, array) \
		__array_cond_foreach_ptr(element_ptr, array, (element_ptr) != NULL)

#define __array_foreach_ptr(element_ptr, array, size) \
		__array_cond_foreach_ptr(element_ptr, array,  \
				(element_ptr) < (array) + (size))

#define __array_cond_foreach_ptr(element_ptr, array, condition) \
	for ((element_ptr) = (array); (condition); ++(element_ptr))

#define __array_foreach(element, array) \
		__array_foreach_element(element, array, \
				GUARD_SUFFIX(__array_foreach_element_ptr))

#define __array_foreach_element(element, array, element_ptr) \
	for(typeof(element) *element_ptr = __extension__ ({ \
				(element) = *(array); (array); \
			}); \
		(element) != NULL; (element) = *(++element_ptr))
