/**
 * @file
 * @brief C standard library header.
 * @details Contains @link #bool boolean @endlink type and #true/#false macros.
 *
 * @date 09.03.10
 * @author Eldar Abusalimov
 */

#ifndef STDBOOL_H_
#define STDBOOL_H_

#ifndef __cplusplus

#if defined(__STDC__) && defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
//typedef _Bool bool;
#define bool _Bool

#else
/**
 * The is no reason to follow strict ABI in kernel code and to define boolean
 * type as 1-byte _Bool. Make it native word sized.
 *
 * TODO (see below) -- Eldar
 * May be I'm not right and it would be better to introduce something like
 * bool_t to avoid confusion with bool defined by C99 standard.
 */
typedef int bool;
#endif

#define false 0
#define true  1

#else /* __cplusplus */

/* Supporting <stdbool.h> in C++ is a GCC extension.  */
#define _Bool   bool
#define bool    bool
#define false   false
#define true    true

#endif /* __cplusplus */

#define __bool_true_false_are_defined 1

#endif /* STDBOOL_H_ */
