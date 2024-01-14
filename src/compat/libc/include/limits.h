/**
 * @file
 * @brief
 *
 * @date 16.11.12
 * @author Anton Bondarev
 * @author Anton Kozlov
 * 		- asm/limits
 */

#ifndef LIMITS_H_
#define LIMITS_H_

#include <asm/limits.h>
#include <framework/mod/options.h>

#ifndef CHAR_BIT
#define CHAR_BIT 8
#endif /* CHAR_BIT */

#ifndef CHAR_MIN
#define CHAR_MIN SCHAR_MIN
#endif /* CHAR_MIN */

#ifndef CHAR_MAX
#define CHAR_MAX SCHAR_MAX
#endif /* CHAR_MAX */

#ifndef SCHAR_MIN
#define SCHAR_MIN -128
#endif /* SCHAR_MIN */

#ifndef SCHAR_MAX
#define SCHAR_MAX 127
#endif /* SCHAR_MAX */

#ifndef UCHAR_MAX
#define UCHAR_MAX 255
#endif /* UCHAR_MAX */

#ifndef WORD_BIT
#define WORD_BIT 32
#endif /* WORD_BIT */

#ifndef INT_MIN
#define INT_MIN (-INT_MAX - 1)
#endif /* INT_MIN */

#ifndef INT_MAX
#define INT_MAX 2147483647
#endif /* INT_MAX */

#ifndef UINT_MAX
#define UINT_MAX 4294967295U
#endif /* UINT_MAX */

#ifndef SHRT_MIN
#define SHRT_MIN (-32768)
#endif /* SHRT_MIN */

#ifndef SHRT_MAX
#define SHRT_MAX 32767
#endif /* SHRT_MAX */

#ifndef USHRT_MAX
#define USHRT_MAX 65535
#endif /* USHRT_MAX */

#ifndef LONG_BIT
#define LONG_BIT 32
#endif /* LONG_BIT */

#ifndef LONG_MIN
#define LONG_MIN (-LONG_MAX - 1)
#endif /* LONG_MIN */

#ifndef LONG_MAX
#define LONG_MAX 2147483647L
#endif /* LONG_MAX */

#ifndef ULONG_MAX
#define ULONG_MAX 4294967295UL
#endif /* ULONG_MAX */

#ifndef LLONG_MIN
#define LLONG_MIN (-LLONG_MAX - 1)
#endif /* LLONG_MIN */

#ifndef LLONG_MAX
#define LLONG_MAX 9223372036854775807LL
#endif /* LLONG_MAX */

#ifndef ULLONG_MAX
#define ULLONG_MAX 18446744073709551615ULL
#endif /* ULLONG_MAX */

#ifdef __EMBOX__
#include <config/embox/compat/libc/limits.h>
#include <config/embox/kernel/task/resource/atexit_api.h>

/* Maximum number of bytes in a filename (not including \0) */
#define NAME_MAX \
	OPTION_MODULE_GET(embox__compat__libc__limits, NUMBER, name_max)

/* Maximum number of bytes in a pathname, including \0 character */
#define PATH_MAX \
	OPTION_MODULE_GET(embox__compat__libc__limits, NUMBER, path_max)

#define ATEXIT_MAX                                                       \
	OPTION_MODULE_GET(embox__kernel__task__resource__atexit_api, NUMBER, \
	    atexit_max)
#else
#define NAME_MAX   32
#define PATH_MAX   128
#define ATEXIT_MAX 0
#endif

/**
 * Maximum number of bytes in a character, for any supported locale.
 * Minimum Acceptable Value: 1
*/
#define MB_LEN_MAX 1

#endif /* LIMITS_H_ */
