/**
 * @file
 * @brief TODO
 *
 * @date 30.05.10
 * @author Eldar Abusalimov
 */

#ifndef UTIL_STRUCTOF_H_
#define UTIL_STRUCTOF_H_

#include <stddef.h>

/**
 * Casts a member out to the containing structure.
 *
 * @param ptr the pointer to the member
 * @param type the type of the container struct
 * @param member the name of the member within the struct
 */
#define structof(ptr, type, member) \
	((type *) ((char *) ptr - offsetof(type, member)))

#endif /* UTIL_STRUCTOF_H_ */
