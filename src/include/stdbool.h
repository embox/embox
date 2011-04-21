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

#if 0
typedef _Bool bool;
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

#endif /* STDBOOL_H_ */
