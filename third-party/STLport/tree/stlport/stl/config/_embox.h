/*
 * _embox.h
 *
 *  Created on: 21 mars 2013
 *      Author: fsulima
 */

#ifndef __STL_CONFIG_EMBOX_H_
#define __STL_CONFIG_EMBOX_H_

#define _STLP_PLATFORM "Embox"

#define _STLP_USE_UNIX_IO

//#define _STLP_NO_WCHAR_T
//#define _STLP_NO_CWCHAR
//#define _STLP_NO_NATIVE_WIDE_FUNCTIONS

//#define _STLP_NO_LONG_DOUBLE
#define _STLP_NO_VENDOR_MATH_L
#define _STLP_NO_VENDOR_MATH_F

#define _STLP_NO_VENDOR_STDLIB_L

#define _STLP_NO_UNCAUGHT_EXCEPT_SUPPORT

// FIXME: This magically resolves problems with namespace importing
//        To resolve later...
#define _STLP_NO_OWN_NAMESPACE

//#define _STLP_USE_PTHREAD_SPINLOCK
/* If not explicitly specified otherwise, work with threads
 */
#if !defined(_STLP_NO_THREADS) && !defined(_REENTRANT)
#  define _REENTRANT
#endif
#if defined(_REENTRANT) && !defined(_PTHREADS)
# define _PTHREADS
#endif
#if defined(_PTHREADS)
#  define _STLP_THREADS
#  define _STLP_PTHREADS
#endif


#ifdef __WINT_TYPE__
typedef __WINT_TYPE__ wint_t;
#else //__WINT_TYPE__
#error __WINT_TYPE__ is not defined
#endif //__WINT_TYPE__


/* Endiannes */
#include <endian.h>
#if !defined(__BYTE_ORDER) || !defined(__LITTLE_ENDIAN) || !defined(__BIG_ENDIAN)
#  error "One of __BYTE_ORDER, __LITTLE_ENDIAN and __BIG_ENDIAN undefined; Fix me!"
#endif

#if ( __BYTE_ORDER == __LITTLE_ENDIAN )
#  define _STLP_LITTLE_ENDIAN 1
#elif ( __BYTE_ORDER == __BIG_ENDIAN )
#  define _STLP_BIG_ENDIAN 1
#else
#  error "__BYTE_ORDER neither __BIG_ENDIAN nor __LITTLE_ENDIAN; Fix me!"
#endif


#endif /* __STL_CONFIG_EMBOX_H_ */
