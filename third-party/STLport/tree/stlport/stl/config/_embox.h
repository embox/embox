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

#define _STLP_NO_WCHAR_T
#define _STLP_NO_CWCHAR

#define _STLP_NO_LONG_DOUBLE
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


// ToDo: endianness
static void __x86_verificator__(void) {
	// This is to make sure this header only compiles on x86
	asm ("mov %cr2, %eax");
}
#define	__LITTLE_ENDIAN	1234
#define	__BIG_ENDIAN	4321
#define	__PDP_ENDIAN	3412
// x86 architecture only
#define __BYTE_ORDER __LITTLE_ENDIAN

#define LITTLE_ENDIAN __LITTLE_ENDIAN
#define BIG_ENGIAN    __BIG_ENDIAN
#define PDP_ENDIAN    __PDP_ENDIAN
#define BYTE_ORDER    __BYTE_ORDER


#endif /* __STL_CONFIG_EMBOX_H_ */
