/**
 * @file
 * @brief C standard library header.
 * @details Contains @link #bool boolean @endlink type and #true/#false macros.
 *
 * @date 09.03.10
 * @author Eldar Abusalimov
 */

#ifndef COMPAT_LIBC_STDBOOL_H_
#define COMPAT_LIBC_STDBOOL_H_

 /* Don't define bool, true, and false in C++, except as a GNU extension. */
 #ifndef __cplusplus
 #define bool _Bool
 #define true 1
 #define false 0
 #elif defined(__GNUC__) && !defined(__STRICT_ANSI__)
 /* Define _Bool as a GNU extension. */
 #define _Bool bool
 #if __cplusplus < 201103L
 /* For C++98, define bool, false, true as a GNU extension. */
 #define bool  bool
 #define false false
 #define true  true
 #endif
 #endif

 #define __bool_true_false_are_defined 1

#endif /* COMPAT_LIBC_STDBOOL_H_ */
