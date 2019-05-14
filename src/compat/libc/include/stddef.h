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

#if __GNUC__ < 4 || defined(__CDT_PARSER__)
# define offsetof(type, member) \
	((size_t) &((type *) 0x0)->member)
#else
# define offsetof(type, member) \
	__builtin_offsetof(type, member)
#endif

#include <defines/null.h>

#include <defines/size_t.h>

#include <defines/wchar_t.h>

#include <defines/ptrdiff_t.h>

typedef long double max_align_t;

#endif /* STDDEF_H_ */
