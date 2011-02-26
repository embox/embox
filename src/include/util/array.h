/**
 * @file
 * @brief Common array utilities and diffuse arrays.
 * @details
 *     <b>Diffuse array</b> is statically allocated array, which is initialized
 * (populated) in multiple compilation units. In other words you can define the
 * array itself in one unit and add elements to it in another unit. Except for
 * the way of definition and initialization, diffuse array is usual array, so
 * it can be declared and used at run time in regular manner.
 *
 *     Just alike for usual arrays there are two general approaches for
 * iterating over (or determining size of) diffuse array.
 *  - At compile time using the array name. See #ARRAY_DIFFUSE_SIZE().
 *  - At run time for those array which has a special element at the end of
 * array so-called terminator element (e.g. @c NULL). See
 * #ARRAY_DIFFUSE_DEF_TERMINATED() and its static derivative.
 *
 *     Considering current use cases and some implementation issues, diffuse
 * arrays are always allocated in read-only data section as if you have defined
 * a regular array with @c const modifier (although you might not have to). To
 * prevent confusion and to take advantage of compiler type checking always
 * include @c const modifier when defining diffuse array with
 * #ARRAY_DIFFUSE_DEF() and its derivatives or when declaring it using
 * @c extern.
 *
 * @date 13.06.2010
 * @author Eldar Abusalimov
 */

#ifndef UTIL_ARRAY_H_
#define UTIL_ARRAY_H_

#include <impl/util/array.h>

/**
 * Defines a new diffuse array.
 *
 * @param element_type the type of array elements
 * @param name the array name which is used to refer the array itself
 *        and to populate it using #ARRAY_DIFFUSE_ADD()
 * @note The @a element_type must include @c const modifier (see general docs).
 */
#define ARRAY_DIFFUSE_DEF(element_type, name) \
		__ARRAY_DIFFUSE_DEF(element_type, name)

/**
 * Does the same as #ARRAY_DIFFUSE_DEF(), except that the array is defined in
 * the file scope and no global symbol is emitted to the resulting object.
 *
 * @param element_type the type of array elements
 * @param name the array name which is used to refer the array itself
 *        and to populate it using #ARRAY_DIFFUSE_ADD()
 * @note The @a element_type must include @c const modifier (see general docs).
 */
#define ARRAY_DIFFUSE_DEF_STATIC(element_type, name) \
		__ARRAY_DIFFUSE_DEF_STATIC(element_type, name)

/**
 * Defines a new diffuse array ended up by the specified @a terminator element.
 *
 * @param element_type the type of array elements
 * @param name the array name which is used to refer the array itself
 *        and to populate it using #ARRAY_DIFFUSE_ADD()
 * @param terminator an element indicating the array end (e.g. @c NULL pointer)
 * @note The @a element_type must include @c const modifier (see general docs).
 */
#define ARRAY_DIFFUSE_DEF_TERMINATED(element_type, name, terminator) \
		__ARRAY_DIFFUSE_DEF_TERMINATED(element_type, name, terminator)

/**
 * Does the same as #ARRAY_DIFFUSE_DEF_TERMINATED(), except that the array is
 * defined in the file scope and no global symbol is emitted to the resulting
 * object.
 *
 * @param element_type the type of array elements
 * @param name the array name which is used to refer the array itself
 *        and to populate it using #ARRAY_DIFFUSE_ADD()
 * @param terminator an element indicating the array end (e.g. @c NULL pointer)
 * @note The @a element_type must include @c const modifier (see general docs).
 */
#define ARRAY_DIFFUSE_DEF_TERMINATED_STATIC(element_type, name, terminator) \
		__ARRAY_DIFFUSE_DEF_TERMINATED_STATIC(element_type, name, terminator)

/**
 * Adds elements to the specified diffuse array previously defined by
 * #ARRAY_DIFFUSE_DEF() or one of its derivatives.
 *
 * @param array_name the name of the diffuse array to which to add elements
 * @param ... the elements to add
 */
#define ARRAY_DIFFUSE_ADD(array_name, ...) \
		__ARRAY_DIFFUSE_ADD(array_name, __VA_ARGS__)

/**
 * Does the same as #ARRAY_DIFFUSE_ADD() but also puts a pointer to head of the
 * added sub-array into a variable with the specified name.
 *
 * @param array_name the name of the diffuse array to which to add elements
 * @param ptr_name the variable name used to refer to the added sub-array
 * @param ... the elements to add
 */
#define ARRAY_DIFFUSE_ADD_NAMED(array_name, ptr_name, ...) \
		__ARRAY_DIFFUSE_ADD_NAMED(array_name, ptr_name, __VA_ARGS__)

/**
 * Gets the length of the specified diffuse array.
 *
 * @param array_name the array to check size for (must be a symbol literal)
 * @return actual number of array elements
 */
#define ARRAY_DIFFUSE_SIZE(array_name) \
		__ARRAY_DIFFUSE_SIZE(array_name)

/**
 * Gets the length of the specified array. The array must be statically
 * defined/declared.
 *
 * @param array_name the array to check size for
 * @return number of array elements
 */
#define ARRAY_SIZE(array_name) \
		__ARRAY_SIZE(array_name)
/**
 * TODO --Alina
 */
#define array_static_foreach_ptr(element_ptr, array) \
		__array_static_foreach_ptr(element_ptr, array)

/**
 * TODO --Alina
 */
#define array_nullterm_foreach_ptr(element_ptr, array) \
		__array_nullterm_foreach_ptr(element_ptr, array)

/**
 * TODO --Alina
 */
#define array_foreach_ptr(element_ptr, array, size) \
		__array_foreach_ptr(element_ptr, array, size)

#endif /* UTIL_ARRAY_H_ */
