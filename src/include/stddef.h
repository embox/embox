/**
 * @file
 * @brief C standard library header.
 * @details Defines the macros #NULL and #offsetof as well
 *          as the types #ptrdiff_t and #size_t.
 *
 * @date 09.03.10
 * @author Eldar Abusalimov
 */

#ifndef STDDEF_H_
#define STDDEF_H_

#include <asm/types.h>

#if __GNUC__ < 4 || defined(__CDT_PARSER__)
# define offsetof(type, member) \
	((size_t) &((type *) 0x0)->member)
#else
# define offsetof(type, member) \
	__builtin_offsetof(type, member)
#endif

#ifndef NULL
#ifndef __cplusplus
#define NULL ((void *)0)
#else /* __cplusplus */
#define NULL 0
#endif /* ! __cplusplus */
#endif

#ifndef __size_t_defined
#define __size_t_defined
typedef __size_t size_t;
#endif /*__size_t_defined */

#ifndef __wchar_t_defined
#define __wchar_t_defined
typedef unsigned short wchar_t;
#endif /*__wchar_t_defined */

#ifndef __ptrdiff_t_defined
#define __ptrdiff_t_defined
typedef __ptrdiff_t ptrdiff_t;
#endif /*__ptrdiff_t_defined */

#endif /* STDDEF_H_ */
