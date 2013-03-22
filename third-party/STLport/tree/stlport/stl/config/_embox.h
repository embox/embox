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

// ToDo: endianness

#endif /* __STL_CONFIG_EMBOX_H_ */
