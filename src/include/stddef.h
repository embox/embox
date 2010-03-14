/**
 * @file
 * @brief C standard library header.
 * @details Defines the macros #NULL and #offsetof as well as the types
 * #ptrdiff_t and #size_t.
 *
 * @date 09.03.2010
 * @author Eldar Abusalimov
 */

#ifndef STDDEF_H_
#define STDDEF_H_

#define offsetof(type, member) ((size_t) &((type *) 0x0)->member)

#include <asm/types.h>

#ifndef NULL
#define NULL ((void *) 0x0)
#endif

#ifndef __size_t_defined
#define __size_t_defined
typedef __size_t size_t;
#endif/*__size_t_defined */

#ifndef __ptrdiff_t_defined
#define __ptrdiff_t_defined
typedef __ptrdiff_t ptrdiff_t;
#endif/*__ptrdiff_t_defined */

#endif /* STDDEF_H_ */
