/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    11.07.2015
 */

#ifndef COMPILER_H_
#define COMPILER_H_

#ifdef __GNUC__

#define _NORETURN __attribute__ ((noreturn))
#define _PRINTF_FORMAT(format_i, arg_start_i) \
	__attribute__ ((format (printf, format_i, arg_start_i)))

#define __attribute_const__ __attribute__((pure))

#else /* __GNUC__ */

#define _NORETURN
#define _PRINTF_FORMAT(format_i, arg_start_i)

#define __attribute_const__

#endif /* __GNUC__ */

#if (!defined(__STDC__) || !defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L) && \
	(!defined(__GNUC_GNU_INLINE__) || !__GNUC_GNU_INLINE__) && \
	(!defined(__GNUC_STDC_INLINE__) || !__GNUC_STDC_INLINE__)
#define inline /* to nothing */
#endif

#ifndef __weak
# define __weak      __attribute__((weak))
#endif

#endif /* COMPILER_H_ */
