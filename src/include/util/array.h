/**
 * @file
 * @brief TODO
 *
 * @date 13.06.2010
 * @author Eldar Abusalimov
 */

#ifndef UTIL_ARRAY_H_
#define UTIL_ARRAY_H_

#include <impl/util/array.h>

#define ARRAY_DIFFUSE_DEF(element_type, name) \
		__ARRAY_DIFFUSE_DEF(element_type, name)

#define ARRAY_DIFFUSE_DEF_STATIC(element_type, name) \
		__ARRAY_DIFFUSE_DEF_STATIC(element_type, name)

#define ARRAY_DIFFUSE_DEF_TERMINATED(element_type, name, terminator) \
		__ARRAY_DIFFUSE_DEF_TERMINATED(element_type, name, terminator)

#define ARRAY_DIFFUSE_DEF_TERMINATED_STATIC(element_type, name, terminator) \
		__ARRAY_DIFFUSE_DEF_TERMINATED_STATIC(element_type, name, terminator)

#define ARRAY_DIFFUSE_ADD(array_name, element) \
		__ARRAY_DIFFUSE_ADD(array_name, element)

#endif /* UTIL_ARRAY_H_ */
