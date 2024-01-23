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

#include <module/embox/util/array_spread.h>

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

/**
 * Analogs for spread array.
 */
#define array_spread_foreach(element, array) \
	  __array_spread_foreach(element, array)
#define array_spread_foreach_ptr(element_ptr, array) \
	  __array_spread_foreach_ptr(element_ptr, array)
#define array_spread_nullterm_foreach(element, array) \
	  __array_spread_nullterm_foreach(element, array)


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

/* Spread arrays and spread-specific iterators. */

/**
 * Defines a new spread array.
 *
 * @param element_type
 *   The type of array elements with optional modifiers.
 *   To control the scope of array definition the standard visibility modifiers
 *   may be used. Thus, without any modifier the array is defined in the global
 *   scope and could be referenced inside other compilation units.
 *   @c static modifier forces the array to be defined in the file scope
 *   and prevents any global symbol to be emitted to the resulting object.
 *   Static spread array cannot be referenced outside the definition file, but
 *   it remains accessible from other compilation units for elements addition
 *   using #ARRAY_SPREAD_ADD() macro and its @link #ARRAY_SPREAD_ADD_NAMED
 *   named derivative @endlink.
 *   Do not forget to specify @c const modifier explicitly (see general docs).
 * @param name
 *   The array name which is used to refer the array itself and to populate it
 *   using #ARRAY_SPREAD_ADD().
 *
 * @note
 *   This command should be used in the file scope, outside of any block.
 * @note
 *   The @a element_type must include @c const modifier (see general docs).
 */
#define ARRAY_SPREAD_DEF(element_type, name) \
	  __ARRAY_SPREAD_DEF(element_type, name)

/**
 * Defines a new spread array ended up by the specified @a terminator element.
 *
 * @param element_type
 *   The type of array elements with optional modifiers.
 * @param name
 *   The array name which is used to refer the array itself and to populate it
 *   using #ARRAY_SPREAD_ADD().
 * @param terminator
 *   An element indicating the array end (e.g. @c NULL pointer).
 *
 * @note
 *   This command should be used in the file scope, outside of any block.
 * @note
 *   The @a element_type must include @c const modifier (see general docs).
 *
 * @see ARRAY_SPREAD_DEF()
 *   More detailed explanation of macro arguments.
 */
#define ARRAY_SPREAD_DEF_TERMINATED(element_type, name, terminator) \
	  __ARRAY_SPREAD_DEF_TERMINATED(element_type, name, terminator)

/**
 * Array spread declaration
 */
#define ARRAY_SPREAD_DECLARE(element_type, name) \
	__ARRAY_SPREAD_DECLARE(element_type, name)

/**
 * Adds elements to the specified spread array.
 *
 * @param array_name
 *   The name of the spread array to which to add elements.
 * @param ...
 *   The elements to add.
 *
 * @note
 *   This command should be used in the file scope, outside of any block.
 */
#define ARRAY_SPREAD_ADD(array_name, ...) \
	  __ARRAY_SPREAD_ADD(array_name, __VA_ARGS__)

/**
 * Does the same as #ARRAY_SPREAD_ADD() but also puts a pointer to head of the
 * added sub-array into a variable with the specified name.
 *
 * @param array_name
 *   The name of the spread array to which to add elements.
 * @param ptr_name
 *   The variable name used to refer to the added sub-array.
 * @param ...
 *   The elements to add.
 *
 * @note
 *   This command should be used in the file scope, outside of any block.
 */
#define ARRAY_SPREAD_ADD_NAMED(array_name, ptr_name, ...) \
	  __ARRAY_SPREAD_ADD_NAMED(array_name, ptr_name, __VA_ARGS__)

/**
 * Gets the length of the specified spread array.
 *
 * @param array_name
 *   The array to check size for (must be a literal symbol).
 * @return
 *   Actual number of array elements including terminator element (if any).
 */
#define ARRAY_SPREAD_SIZE(array_name) \
	  __ARRAY_SPREAD_SIZE(array_name)

/**
 * Gets the length of the specified spread array without taking into an
 * account a terminator element (if any).
 *
 * @param array_name
 *   The array to check size for (must be a literal symbol).
 * @return
 *   Number of array elements except terminating (if any). If the target array
 *   is not terminated then the result is the same as of #ARRAY_SPREAD_SIZE().
 */
#define ARRAY_SPREAD_SIZE_IGNORE_TERMINATING(array_name) \
	  __ARRAY_SPREAD_SIZE_IGNORE_TERMINATING(array_name)

#endif /* UTIL_ARRAY_H_ */
