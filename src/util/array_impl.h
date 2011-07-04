/**
 * @file
 * @brief Implements array iterators and linker-powered distributed arrays.
 *
 * @date 13.06.10
 * @author Eldar Abusalimov
 */

#ifndef UTIL_ARRAY_IMPL_H_
#define UTIL_ARRAY_IMPL_H_

#include <stddef.h>

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
#define __ARRAY_SPREAD_SECTION(array_nm, order_tag) \
	".array_spread." #array_nm order_tag ".rodata"

/* Every array entry, group of entries or marker symbols are backed by an
 * individual array (empty for markers) defined as follows. */
#define __ARRAY_SPREAD_ENTRY_DEF(type, array_nm, entry_nm, section_order_tag) \
	/* __extension__ bypasses compiler warnings about empty arrays. */        \
	__extension__ const type entry_nm[] __attribute__ ((used,                 \
			section(__ARRAY_SPREAD_SECTION(array_nm, section_order_tag)),     \
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
	__extension__ const element_t array_nm[] __attribute__             \
		/* Some versions of GCC do not take into an account section    \
		 * attribute if it appears after the definition. */            \
			((section(__ARRAY_SPREAD_SECTION(array_nm, "0_head")))) =  \
		{ /* Empty anchor to the array head. */ };                     \
	__ARRAY_SPREAD_ENTRY_DEF(element_t, array_nm, array_nm, "0_head"); \
		/* Now the head has got all necessary attributes. */           \
	__ARRAY_SPREAD_PRIVATE_DEF(element_t, array_nm, term, "8_term") =  \
		{ terminator /* Terminating element (if any). */ };            \
	__ARRAY_SPREAD_PRIVATE_DEF(element_t, array_nm, tail, "9_tail") =  \
		{ /* Empty anchor at the very end of the array. */ }

#define __ARRAY_SPREAD_DEF(element_t, array_nm) \
	__ARRAY_SPREAD_DEF_TERMINATED(element_t, array_nm, /* empty */)

/* Spread element addition. */

#define __ARRAY_SPREAD_ADD_NAMED(array_nm, ptr_nm, ...) \
	__ARRAY_SPREAD_ENTRY_DEF(static typeof(array_nm[0]), array_nm, \
		ptr_nm, "1_body") = { __VA_ARGS__ }

#define __ARRAY_SPREAD_ADD(array_nm, ...) \
	__ARRAY_SPREAD_ADD_NAMED(array_nm,                          \
		MACRO_GUARD(__ARRAY_SPREAD_PRIVATE(array_nm, element)), \
		__VA_ARGS__)

/* Spread size calculations. */

#define __ARRAY_SPREAD_SIZE(array_name) \
		__ARRAY_SPREAD_SIZE_MARKER(array_name, tail)

#define __ARRAY_SPREAD_SIZE_IGNORE_TERMINATING(array_name) \
		__ARRAY_SPREAD_SIZE_MARKER(array_name, term)

#define __ARRAY_SPREAD_SIZE_MARKER(array_name, marker) __extension__ ({ \
		extern typeof(array_name) __ARRAY_SPREAD_PRIVATE(array_name,marker); \
		(size_t) (__ARRAY_SPREAD_PRIVATE(array_name, marker) - array_name);  \
	})

/* Static array size. */

#define __ARRAY_SIZE(array) \
	(sizeof(array) / sizeof(*(array)))

/* Scalar foreach iterations. */

#define __array_foreach(element, array, size) \
		__array_range_foreach(element, array, (array) + (size))

#define __array_static_foreach(element, array) \
		__array_foreach(element, array, ARRAY_SIZE(array))

#define __array_spread_foreach(element, array) \
		__array_foreach(element, array, ARRAY_SPREAD_SIZE(array))

#define __array_range_foreach(element, array_begin, array_end) \
		__array_range_foreach__(element, array_begin, array_end, \
				MACRO_GUARD(__array_elem_ptr), MACRO_GUARD(__array_end))

#define __array_range_foreach__(element, array_begin, array_end, _ptr, _end) \
	for (const typeof(element) *_ptr = (array_begin), \
				*_end = __extension__ ({              \
					(element) = *_ptr; (array_end);   \
				});                                   \
			_ptr < _end;                              \
			(element) = *(++_ptr))

#define __array_nullterm_foreach(element, array) \
	__array_terminated_foreach(element, array, (typeof(element)) NULL)

#define __array_terminated_foreach(element, array, terminator) \
	__array_terminated_foreach__(element, array, terminator, \
		MACRO_GUARD(__array_elem_ptr), MACRO_GUARD(__array_term))

#define __array_terminated_foreach__(element, array, terminator, _ptr, _term) \
	for (const typeof(element) *_ptr = (array), \
				_term = (terminator);           \
			((element) = *_ptr) != _term;       \
			++_ptr)

#define __array_cond_foreach(element, array, cond) \
		__array_cond_foreach__(element, array, cond, \
				MACRO_GUARD(__array_elem_ptr))

#define __array_cond_foreach__(element, array, cond, _ptr) \
	for (const typeof(element) *_ptr = __extension__ ({           \
					const typeof(element) *__ptr_tmp__ = (array); \
					(element) = *__ptr_tmp__; __ptr_tmp__;        \
				});                                               \
			(cond);                                               \
			(element) = *(++_ptr))

/* Pointer foreach iterations. */

#define __array_foreach_ptr(element_ptr, array, size) \
		__array_range_foreach_ptr(element_ptr, array, (array) + (size))

#define __array_static_foreach_ptr(element_ptr, array) \
		__array_foreach_ptr(element_ptr, array, ARRAY_SIZE(array))

#define __array_spread_foreach_ptr(element_ptr, array) \
		__array_foreach_ptr(element_ptr, array, ARRAY_SPREAD_SIZE(array))

#define __array_range_foreach_ptr(element_ptr, array_begin, array_end) \
		__array_range_foreach_ptr__(element_ptr, array_begin, array_end, \
				MACRO_GUARD(__array_end))

#define __array_range_foreach_ptr__(element_ptr, array_begin, array_end, _end)\
	for (typeof(element_ptr) _end = __extension__ ({  \
					(element_ptr) = (array_begin);    \
					(array_end);                      \
				});                                   \
			(element_ptr) < _end;                     \
			++(element_ptr))

#define __array_cond_foreach_ptr(element_ptr, array, cond) \
	for ((element_ptr) = (array); \
			(cond);               \
			++(element_ptr))

#endif /* UTIL_ARRAY_IMPL_H_ */
