/**
 * @file
 * @brief Implements array iterators and linker-powered distributed arrays.
 *
 * @date 13.06.10
 * @author Eldar Abusalimov
 */

#ifndef UTIL_ARRAY_IMPL_H_
#define UTIL_ARRAY_IMPL_H_

#ifndef __LDS__

#include <stddef.h>
#include <sys/cdefs.h>

#include <util/macro.h>

/*
 * For each spread array four sections are created. These sections are then
 * placed together strictly successively each after each (notice their names):
 *  - "0_head" section contains a symbol for the name of the array itself,
 *  - "1_body" hosts elements of the array (excepting terminator, if any),
 *  - "8_term" contains a terminator element, if any, and
 *  - "9_tail" holds a symbol for the array end marker.
 *
 * All array_spread-related sections are linked using something
 * like *(SORT(.array_spread.*.rodata)).
 * SORT guarantees that the wildcard is expanded in the lexicographical order
 * and all the sections will be placed in the right order according to their
 * names.
 */

/* The relative placement of sections within a particular array is controlled
 * by the value of order_tag argument. */
#if defined __LCC__
#define __ARRAY_SPREAD_SECTION(array_nm, order_tag) \
	".array_spread." #array_nm order_tag ".rodata,\"a\";#"
#else
#define __ARRAY_SPREAD_SECTION(array_nm, order_tag) \
	".array_spread." #array_nm order_tag ".rodata,\"a\",%progbits;#"
#endif

/* Every array entry, group of entries or marker symbols are backed by an
 * individual array (empty for markers) defined as follows. */
#define __ARRAY_SPREAD_ENTRY_DEF(type, array_nm, entry_nm, section_order_tag) \
	type volatile entry_nm[] __attribute__ ((used,                  \
			section(__ARRAY_SPREAD_SECTION(array_nm, section_order_tag)), \
			aligned(__alignof__(array_nm[0]))))

/* Spread arrays implementation-private entries are named as follows
 * to prevent namespace pollution. */
#define __ARRAY_SPREAD_PRIVATE(array_nm, private_nm) \
	__array_spread__##array_nm##__##private_nm

#define __ARRAY_SPREAD_PRIVATE_DEF(type, array_nm, private_nm, \
		section_order_tag) \
	__ARRAY_SPREAD_ENTRY_DEF(type, array_nm,              \
			__ARRAY_SPREAD_PRIVATE(array_nm, private_nm), \
			section_order_tag)

/* Here goes API implementation. */

/* Spread definition. */

/* Each element including auxiliary entries must be properly aligned.
 * The alignment is taken from the head entry which has the same type as
 * array elements.
 * This implies that the array itself (head entry) must be declared twice:
 *  - first time is for define its type, and
 *  - second time - to add necessary attributes (alignment in particular). */
#define __ARRAY_SPREAD_DEF_TERMINATED(element_t, array_nm, terminator) \
	element_t volatile array_nm[] __attribute__ ((used,          \
		/* Some versions of GCC do not take into an account section    \
		 * attribute if it appears after the definition. */            \
			section(__ARRAY_SPREAD_SECTION(array_nm, "0_head")))) =    \
		{ /* Empty anchor to the array head. */ };                     \
	__ARRAY_SPREAD_PRIVATE_DEF(element_t, array_nm, term, "8_term") =  \
		{ terminator /* Terminating element (if any). */ };            \
	__ARRAY_SPREAD_PRIVATE_DEF(element_t, array_nm, tail, "9_tail") =  \
		{ /* Empty anchor at the very end of the array. */ }

#define __ARRAY_SPREAD_DEF(element_t, array_nm) \
	__ARRAY_SPREAD_DEF_TERMINATED(element_t, array_nm, /* empty */)

/* Spread declaration */

#define __ARRAY_SPREAD_DECLARE(element_t, array_nm) \
	EXTERN_C element_t volatile array_nm[]

/* Spread element addition. */

#define __ARRAY_SPREAD_ADD_NAMED_ORDERED(array_nm, ptr_nm, order, ...) \
	__ARRAY_SPREAD_ENTRY_DEF(static typeof(array_nm[0]), array_nm, \
		ptr_nm, "1_body" #order) = { __VA_ARGS__ }

#define __ARRAY_SPREAD_ADD_NAMED(array_nm, ptr_nm, ...) \
	__ARRAY_SPREAD_ADD_NAMED_ORDERED(array_nm, ptr_nm, , __VA_ARGS__)

#define __ARRAY_SPREAD_GUARD(array_nm) \
	MACRO_GUARD(__ARRAY_SPREAD_PRIVATE(array_nm, element))

#define __ARRAY_SPREAD_ADD(array_nm, ...) \
	__ARRAY_SPREAD_ADD_NAMED_ORDERED(array_nm,                  \
		__ARRAY_SPREAD_GUARD(array_nm),                        \
		, __VA_ARGS__)

#define __ARRAY_SPREAD_ADD_ORDERED(array_nm, order, ...) \
	__ARRAY_SPREAD_ADD_NAMED_ORDERED(array_nm,                  \
		__ARRAY_SPREAD_GUARD(array_nm),                        \
		order, __VA_ARGS__)

/* Spread size calculations. */

#define __ARRAY_SPREAD_SIZE(array_nm) \
		__ARRAY_SPREAD_SIZE_MARKER(array_nm, tail)

#define __ARRAY_SPREAD_SIZE_IGNORE_TERMINATING(array_nm) \
		__ARRAY_SPREAD_SIZE_MARKER(array_nm, term)

#define __ARRAY_SPREAD_SIZE_MARKER(array_nm, marker) \
	({ \
		extern typeof(array_nm) __ARRAY_SPREAD_PRIVATE(array_nm, marker); \
		(size_t) (__ARRAY_SPREAD_PRIVATE(array_nm, marker) - array_nm);   \
	})

/* Scalar foreach iterations. */

#define __array_foreach(element, array, size) \
	__array_foreach_nm(element, array, size, MACRO_GUARD(__ptr))

#define __array_foreach_nm(element, array, size, _ptr) \
	for (const typeof(element) *_ptr = (array), *_end = _ptr + (size); \
			(_ptr < _end) && (((element) = *_ptr) || 1); ++_ptr)

#define __array_range_foreach(element, array_begin, array_end) \
	__array_range_foreach_nm(element, array_begin, array_end, \
			MACRO_GUARD(__ptr), MACRO_GUARD(__end))

#define __array_range_foreach_nm(element, array_begin, array_end, _ptr, _end) \
	for (const typeof(element) *_ptr = (array_begin), *_end = (array_end); \
			(_ptr < _end) && (((element) = *_ptr) || 1); ++_ptr)

#define __array_nullterm_foreach(element, array) \
	__array_nullterm_foreach_nm(element, array, \
			MACRO_GUARD(__ptr))

#define __array_nullterm_foreach_nm(element, array, _ptr) \
	for (const typeof(element) *_ptr = (array); ((element) = *_ptr); ++_ptr)

/* Pointer foreach iterations. */

#define __array_foreach_ptr(element_ptr, array, size) \
	__array_range_foreach_ptr_nm(element_ptr, array, \
			MACRO_GUARD(__ptr) + (size), \
			MACRO_GUARD(__ptr), MACRO_GUARD(__end))

#define __array_range_foreach_ptr(element_ptr, array_begin, array_end) \
	__array_range_foreach_ptr_nm(element_ptr, array_begin, array_end, \
			MACRO_GUARD(__ptr), MACRO_GUARD(__end))

#define __array_range_foreach_ptr_nm(element_ptr, array_begin, array_end, \
		_ptr, _end) \
	for (typeof(element_ptr) _ptr = (array_begin), _end = (array_end); \
			(_ptr < _end) && ((element_ptr) = _ptr); ++_ptr)

/* Spread array iterators. */

#define __array_spread_foreach(element, array)                          \
	__array_spread_foreach_nm(element, array, ARRAY_SPREAD_SIZE(array), \
	    MACRO_GUARD(__ptr), MACRO_GUARD(__end))

#define __array_spread_foreach_nm(element, array, size, _ptr, _end) \
	for (typeof(element) volatile const *_ptr = (array),            \
	                                    *_end = _ptr + (size);      \
	     (_ptr < _end) && (((element) = *_ptr) || 1); ++_ptr)

#define __array_spread_foreach_reverse(element, array) \
	__array_spread_foreach_reverse_nm(element, array,  \
	    ARRAY_SPREAD_SIZE(array), MACRO_GUARD(__ptr), MACRO_GUARD(__end))

#define __array_spread_foreach_reverse_nm(element, array, size, _ptr, _end) \
	for (typeof(element) volatile const *_end = (array),                    \
	                                    *_ptr = _end + (size) - 1;          \
	     (_ptr >= _end) && (((element) = *_ptr) || 1); --_ptr)

#define __array_spread_foreach_ptr(element_ptr, array)     \
	__array_spread_foreach_ptr_nm(element_ptr, array,      \
			MACRO_GUARD(__ptr) + ARRAY_SPREAD_SIZE(array), \
			MACRO_GUARD(__ptr), MACRO_GUARD(__end))

#define __array_spread_foreach_ptr_nm(element_ptr, array_begin,           \
		array_end, _ptr, _end)                                            \
	for (typeof(*element_ptr) volatile const *_ptr = (array_begin),       \
				*_end = (array_end);                                      \
			(_ptr < _end) && ((element_ptr) = (typeof(element_ptr))_ptr); \
			++_ptr)

#define __array_spread_nullterm_foreach(element, array) \
	__array_spread_nullterm_foreach_nm(element, array,  \
			MACRO_GUARD(__ptr))

#define __array_spread_nullterm_foreach_nm(element, array, _ptr) \
	for (typeof(element) volatile const *_ptr = (array);         \
			((element) = *_ptr); ++_ptr)


/* Help Eclipse CDT. */
#ifdef __CDT_PARSER__

# undef  __ARRAY_SPREAD_DEF_TERMINATED
# define __ARRAY_SPREAD_DEF_TERMINATED(element_t, array_nm, terminator) \
	element_t volatile array_nm[]

# undef  __ARRAY_SPREAD_ADD
# define __ARRAY_SPREAD_ADD(array_nm, ...) \
	typedef typeof(array_nm[0]) __array_spread_element_placeholder

# undef  __ARRAY_SPREAD_ADD_NAMED
# define __ARRAY_SPREAD_ADD_NAMED(array_nm, ptr_nm, ...) \
	static typeof(array_nm[0]) volatile const ptr_nm[]

#endif /* __CDT_PARSER__ */

#endif /* __LDS__ */

#endif /* UTIL_ARRAY_IMPL_H_ */
