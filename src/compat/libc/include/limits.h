/**
 * @file
 * @brief
 *
 * @date 16.11.12
 * @author Anton Bondarev
 * @author Anton Kozlov
 * 		- asm/limits
 */

#ifndef COMPAT_LIBC_LIMITS_H_
#define COMPAT_LIBC_LIMITS_H_

#define CHAR_BIT __CHAR_BIT__
#define WORD_BIT (__SIZEOF_INT__ * 8)
#define LONG_BIT (__SIZEOF_LONG__ * 8)

#define CHAR_MAX  __SCHAR_MAX__
#define SCHAR_MAX __SCHAR_MAX__
#define SHRT_MAX  __SHRT_MAX__
#define INT_MAX   __INT_MAX__
#define LONG_MAX  __LONG_MAX__
#define LLONG_MAX __LONG_LONG_MAX__

#define CHAR_MIN  (-CHAR_MAX - 1)
#define SCHAR_MIN (-SCHAR_MAX - 1)
#define SHRT_MIN  (-SHRT_MAX - 1)
#define INT_MIN   (-INT_MAX - 1)
#define LONG_MIN  (-LONG_MAX - 1)
#define LLONG_MIN (-LLONG_MAX - 1)

#define UCHAR_MAX  (2U * SCHAR_MAX + 1)
#define USHRT_MAX  (2U * SHRT_MAX + 1)
#define UINT_MAX   (2U * INT_MAX + 1)
#define ULONG_MAX  (2UL * LONG_MAX + 1)
#define ULLONG_MAX (2ULL * LLONG_MAX + 1)

#ifdef __EMBOX__
#include <framework/mod/options.h>

#include <config/embox/compat/libc/limits.h>

/* Maximum length of argument to the exec functions including environment data */
#define ARG_MAX OPTION_MODULE_GET(embox__compat__libc__limits, NUMBER, arg_max)

/* Maximum number of bytes in a filename (not including \0) */
#define NAME_MAX \
	OPTION_MODULE_GET(embox__compat__libc__limits, NUMBER, name_max)

/* Maximum number of bytes in a pathname, including \0 character */
#define PATH_MAX \
	OPTION_MODULE_GET(embox__compat__libc__limits, NUMBER, path_max)

/* Maximum number of functions that may be registered with atexit() */
#define ATEXIT_MAX \
	OPTION_MODULE_GET(embox__compat__libc__limits, NUMBER, atexit_max)
#else
#define ARG_MAX    16
#define NAME_MAX   32
#define PATH_MAX   128
#define ATEXIT_MAX 0
#endif

/**
 * Maximum number of bytes in a character, for any supported locale.
 * Minimum Acceptable Value: 1
*/
#define MB_LEN_MAX 1

#endif /* COMPAT_LIBC_LIMITS_H_ */
