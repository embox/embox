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

#ifndef CHAR_BIT
#define CHAR_BIT 	8
#endif /* CHAR_BIT */

#ifndef CHAR_MIN
#define CHAR_MIN 	SCHAR_MIN
#endif /* CHAR_MIN */

#ifndef CHAR_MAX
#define CHAR_MAX 	SCHAR_MAX
#endif /* CHAR_MAX */

#ifndef SCHAR_MIN
#define SCHAR_MIN 	-128
#endif /* SCHAR_MIN */

#ifndef SCHAR_MAX
#define SCHAR_MAX 	127
#endif /* SCHAR_MAX */

#ifndef UCHAR_MAX
#define UCHAR_MAX 	255
#endif /* UCHAR_MAX */


#ifndef WORD_BIT
#define WORD_BIT 	32
#endif /* WORD_BIT */

#ifndef INT_MIN
#define INT_MIN		(-INT_MAX - 1)
#endif /* INT_MIN */

#ifndef INT_MAX
#define INT_MAX		2147483647
#endif /* INT_MAX */

#ifndef UINT_MAX
#define UINT_MAX	4294967295U
#endif /* UINT_MAX */


#ifndef SHRT_MIN
#define SHRT_MIN	(-32768)
#endif /* SHRT_MIN */

#ifndef SHRT_MAX
#define SHRT_MAX	32767
#endif /* SHRT_MAX */

#ifndef USHRT_MAX
#define USHRT_MAX	65535
#endif /* USHRT_MAX */


#ifndef LONG_BIT
#define LONG_BIT	32
#endif /* LONG_BIT */

#ifndef LONG_MIN
#define LONG_MIN	(-LONG_MAX - 1)
#endif /* LONG_MIN */

#ifndef LONG_MAX
#define LONG_MAX	2147483647L
#endif /* LONG_MAX */

#ifndef ULONG_MAX
#define ULONG_MAX	4294967295UL
#endif /* ULONG_MAX */


#ifndef LLONG_MIN
#define LLONG_MIN	(-LLONG_MAX - 1)
#endif /* LLONG_MIN */

#ifndef LLONG_MAX
#define LLONG_MAX	9223372036854775807LL
#endif /* LLONG_MAX */


#ifndef ULLONG_MAX
#define ULLONG_MAX	18446744073709551615ULL
#endif /* ULLONG_MAX */

#include <framework/mod/options.h>
/**
 * Pathname Variable Values
 */
#ifdef __EMBOX__
#include <module/embox/compat/libc/limits.h>
#include <config/embox/kernel/task/resource/atexit_api.h>
#endif
/* Maximum number of bytes in a filename (not including \0) */
#define NAME_MAX \
	OPTION_MODULE_GET(embox__compat__libc__limits, NUMBER, name_max)

/* Maximum number of bytes in a pathname, including \0 character */
#define PATH_MAX \
	OPTION_MODULE_GET(embox__compat__libc__limits, NUMBER, path_max)

#define ATEXIT_MAX \
	OPTION_MODULE_GET(embox__kernel__task__resource__atexit_api, NUMBER, atexit_max)

/*
Maximum number of bytes in a character, for any supported locale.
    Minimum Acceptable Value: 1
*/
#define MB_LEN_MAX 1

//TODO life is pain
#include <ctype.h>

/* POSIX/SUS minimum requirements.
 * These numbers come from SUS and are host independent. */

/*
#define _POSIX_AIO_LISTIO_MAX   2
#define _POSIX_AIO_MAX          1
#define _POSIX_ARG_MAX          4096
#define _POSIX_CHILD_MAX        25
#define _POSIX_CLOCKRES_MIN     20000000
#define _POSIX_DELAYTIMER_MAX   32
#define _POSIX_HOST_NAME_MAX    255
#define _POSIX_LINK_MAX         8
#define _POSIX_LOGIN_NAME_MAX   9
#define _POSIX_MAX_CANON        255
#define _POSIX_MAX_INPUT        255
#define _POSIX_MQ_OPEN_MAX      8
#define _POSIX_MQ_PRIO_MAX      32
#define _POSIX_NAME_MAX         14
#define _POSIX_NGROUPS_MAX      8
#define _POSIX_OPEN_MAX         20
*/
#define _POSIX_PATH_MAX         256
/*
#define _POSIX_PIPE_BUF         512
#define _POSIX_RE_DUP_MAX       255
#define _POSIX_RTSIG_MAX        8
#define _POSIX_SEM_NSEMS_MAX    256
#define _POSIX_SEM_VALUE_MAX    32767
#define _POSIX_SIGQUEUE_MAX     32
#define _POSIX_SSIZE_MAX        32767
#define _POSIX_STREAM_MAX       8
#define _POSIX_SS_REPL_MAX      4
#define _POSIX_SYMLINK_MAX      255
#define _POSIX_SYMLOOP_MAX      8
#define _POSIX_THREAD_DESTRUCTOR_ITERATIONS 4
#define _POSIX_THREAD_KEYS_MAX  128
#define _POSIX_THREAD_THREADS_MAX 64
#define _POSIX_TIMER_MAX        32
#define _POSIX_TRACE_EVENT_NAME_MAX 30
#define _POSIX_TRACE_NAME_MAX   8
#define _POSIX_TRACE_SYS_MAX    8
#define _POSIX_TRACE_USER_EVENT_MAX 32
#define _POSIX_TTY_NAME_MAX     9
#define _POSIX_TZNAME_MAX       6
#define _POSIX2_BC_BASE_MAX     99
#define _POSIX2_BC_DIM_MAX      2048
#define _POSIX2_BC_SCALE_MAX    99
#define _POSIX2_BC_STRING_MAX   1000
#define _POSIX2_CHARCLASS_NAME_MAX 14
#define _POSIX2_COLL_WEIGHTS_MAX 2
#define _POSIX2_EXPR_NEST_MAX   32
#define _POSIX2_LINE_MAX        2048
#define _POSIX2_RE_DUP_MAX      255

#define _XOPEN_IOV_MAX          16
#define _XOPEN_NAME_MAX         255
#define _XOPEN_PATH_MAX         1024
*/
#endif /* LIMITS_H_ */
