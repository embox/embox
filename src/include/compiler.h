/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    11.07.2015
 */

#ifndef EMBOX_COMPILER_H_
#define EMBOX_COMPILER_H_

/* Macro to test the version of gcc */
#if defined __GNUC__ && defined __GNUC_MINOR__
#undef __GNUC_PREREQ
#define __GNUC_PREREQ(maj, min) \
	((__GNUC__ << 16) + __GNUC_MINOR__ >= ((maj) << 16) + (min))
#else
#undef __GNUC_PREREQ
#define __GNUC_PREREQ(maj, min) 0
#endif

#ifdef __GNUC__
#define _NORETURN __attribute__((noreturn))
#else
#define _NORETURN
#endif

#ifdef __GNUC__
#define _PRINTF_FORMAT(format_i, arg_start_i) \
	__attribute__((format(printf, format_i, arg_start_i)))
#else
#define _PRINTF_FORMAT(format_i, arg_start_i)
#endif

#ifdef __GNUC__
#define __attribute_const__ __attribute__((pure))
#else
#define __attribute_const__
#endif

#if (!defined(__STDC__) || !defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L) \
    && (!defined(__GNUC_GNU_INLINE__) || !__GNUC_GNU_INLINE__)                       \
    && (!defined(__GNUC_STDC_INLINE__) || !__GNUC_STDC_INLINE__)
#define inline /* to nothing */
#endif

#ifndef __weak
#define __weak __attribute__((weak))
#endif

#if defined(__clang__)
#define __attribute_no_ubsan __attribute__((no_sanitize("undefined")))
#elif defined(__GNUC__)
#define __attribute_no_ubsan __attribute__((no_sanitize_undefined))
#else
#define __attribute_no_ubsan
#endif

#endif /* EMBOX_COMPILER_H_ */
