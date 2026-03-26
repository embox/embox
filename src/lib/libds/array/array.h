/**
 * @file
 * @brief Common array utilities and spread arrays.
 * @details
 * TODO array_foreach general overview. -- Eldar
 *     <b>Spread array</b> is statically allocated array, which is initialized
 * (populated) in multiple compilation units. In other words you can define the
 * array itself in one unit and add elements to it in another unit. Except for
 * the way of definition and initialization, spread array is usual array, so
 * it can be declared and used at run time in regular manner.
 *
 *     Just alike for usual arrays there are two general approaches for
 * iterating over (or determining size of) spread array.
 *  - At compile time using the array name. See #ARRAY_SPREAD_SIZE().
 *  - At run time for those array which has a special element at the end of
 * array so-called terminator element (e.g. @c NULL). See
 * #ARRAY_SPREAD_DEF_TERMINATED().
 *
 *     Considering current use cases and some implementation issues, spread
 * arrays are always allocated in read-only data section as if you have defined
 * a regular array with @c const modifier (although you might not have to). To
 * prevent confusion and to take advantage of compiler type checking <b>always
 * include @c const modifier to element type</b> when defining spread array
 * with #ARRAY_SPREAD_DEF() and its derivatives or when declaring it using
 * @c extern.
 *
 * @date 13.06.10
 * @author Eldar Abusalimov
 */

#ifndef UTIL_ARRAY_H_
#define UTIL_ARRAY_H_

#include <module/embox/lib/libds_array.h>

/* Foreach iterators for generic arrays: with given size or null-terminated. */

/**
 * Iterates over the specified @a array of scalar values with given number of
 * elements.
 *
 * @param element
 *   Iteration variable which takes a value of each element of the target
 *   array one by one, up to the [@a size - 1]'th element.
 * @param array
 *   The array to iterate over.
 *   Evaluated only once that allows the argument to have side effects.
 * @param size
 *   The array size.
 *   Alike @a array the argument is evaluated only once too.
 */
#define array_foreach(element, array, size) \
	  __array_foreach(element, array, size)

/**
 * Iterates over the specified array of (possibly) non-scalar values with given
 * number of elements using a pointer to access the elements.
 *
 * @param element_ptr
 *   Iteration variable which takes a value pointing to each element of the
 *   target array, starting from the value of the @a array head end advancing
 *   up to the pointer to [@a size - 1]'th element.
 * @param array
 *   The array to iterate over.
 * @param size
 *   The array size. Evaluated once.
 *
 * @see array_foreach()
 */
#define array_foreach_ptr(element_ptr, array, size) \
	  __array_foreach_ptr(element_ptr, array, size)

/**
 * Iterates over the specified @a array of scalar values until @c NULL is
 * reached.
 *
 * @param element
 *   Iteration variable which takes a value of each element of the target
 *   array one by one.
 * @param array
 *   The array to iterate over.
 *   Evaluated only once that allows the argument to have side effects.
 */
#define array_nullterm_foreach(element, array) \
	  __array_nullterm_foreach(element, array)

/**
 * Iterates over an array starting at @a array_begin and advancing until
 * @a array_end is reached.
 *
 * @param element
 *   Iteration variable which takes a value of each element of the target
 *   array one by one.
 * @param array_begin
 *   The pointer to start the iteration from.
 *   Evaluated only once allowing the argument to have side effects.
 * @param array_end
 *   The pointer denoting the array bound.
 *   Alike @a array_begin it is evaluated only once too.
 */
#define array_range_foreach(element, array_begin, array_end) \
	  __array_range_foreach(element, array_begin, array_end)

/**
 * Iterates over an array using a pointer to access its elements, starting at
 * @a array_begin and advancing until @a array_end is reached.
 *
 * @param element_ptr
 *   Iteration variable which takes a value pointing to each element of the
 *   target array one by one.
 * @param array_begin
 *   The pointer to start the iteration from. Evaluated once.
 * @param array_end
 *   The pointer denoting the array bound.  Evaluated once.
 *
 * @see array_range_foreach()
 */
#define array_range_foreach_ptr(element_ptr, array_begin, array_end) \
	  __array_range_foreach_ptr(element_ptr, array_begin, array_end)


/* Static arrays with their size known at the compile-time. */

/**
 * Gets the length of the specified @a array.
 * The macro can be used only if the array length is known at the compile-time.
 *
 * @param array
 *   The array to check size for.
 * @return
 *   Number of array elements.
 *
 * @note
 *   The array must be statically defined/declared, otherwise an incorrect
 *   result is returned without any error or warning.
 */
#define ARRAY_SIZE(array) \
	(sizeof(array) / sizeof(*(array)))

#include <lib/libds/array_spread.h>

#endif /* UTIL_ARRAY_H_ */
