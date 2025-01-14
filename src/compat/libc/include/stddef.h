/**
 * @file
 * @brief C standard library header.
 * @details Defines the macros #NULL and #offsetof as well
 *          as the types #ptrdiff_t and #size_t.
 *
 * @date 09.03.10
 * @author Eldar Abusalimov
 */

#ifndef COMPAT_LIBC_STDDEF_H_
#define COMPAT_LIBC_STDDEF_H_

#if __GNUC__ < 4 || defined(__CDT_PARSER__)
#define offsetof(type, member) ((size_t) & ((type *)0x0)->member)
#else
#define offsetof(type, member) __builtin_offsetof(type, member)
#endif

#ifndef __cplusplus
#define NULL ((void *)0)
#else /* __cplusplus */
#define NULL 0
#endif /* ! __cplusplus */

#ifndef __ASSEMBLER__

typedef __SIZE_TYPE__ size_t;
typedef __PTRDIFF_TYPE__ ptrdiff_t;

/* wchar_t is C++ built-in type */
#ifndef __cplusplus
typedef __WCHAR_TYPE__ wchar_t;
#endif

typedef long double max_align_t;

#endif /* !__ASSEMBLER__ */

#endif /* COMPAT_LIBC_STDDEF_H_ */
