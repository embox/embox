/**
 * @file
 * @brief Macros for aligning values up to the power of two boundaries.
 *
 * @date 04.06.2010
 * @author Eldar Abusalimov
 */

#ifndef UTIL_BINALIGN_H_
#define UTIL_BINALIGN_H_

#include <impl/util/binalign.h>

/**
 * Aligns the @c x value up to the @c 2^power boundary.
 *
 * @param x the value to align up to the boundary
 * @param power the order of two to construct boundary for
 *
 * @return the minimal @c 2^power aligned value closest to the @c x
 */
#define binalign(x, power) __binalign(x, power)

/**
 * Aligns the @c x value up to the specified boundary.
 *
 * @param x the value to be aligned
 * @param bound the two-powered boundary
 *
 * @return the minimal @c bound aligned value closest to the @c x
 *
 * @note the @c bound must the power of two value
 */
#define binalign_bound(x, bound) __binalign_bound(x, bound)

#ifndef __ASSEMBLER__

/**
 * Checks whether the specified value is power of two aligned.
 *
 * @param x the value to check the alignment for
 * @param power the order of two
 *
 * @return the check result
 *
 * @see #binalign()
 */
#define binalign_check(x, power) __binalign_check(x, power)

/**
 * Checks whether @c x is aligned to the specified boundary.
 *
 * @param x the value to check the alignment for
 * @param bound the two-powered boundary
 *
 * @return the check result
 *
 * @note the @c bound must the power of two value
 * @see #binalign_bound()
 */
#define binalign_check_bound(x, bound) __binalign_check_bound(x, bound)

#endif /* __ASSEMBLER__ */

#endif /* UTIL_BINALIGN_H_ */
