/**
 * @file
 * @brief Macros for aligning values up to the power of two boundaries.
 *
 * @date 04.06.2010
 * @author Eldar Abusalimov
 */

#ifndef UTIL_BINALIGN_H_
#define UTIL_BINALIGN_H_

/**
 * Aligns the @c x value up to the @c 2^power boundary.
 *
 * @param x the value to align up to the boundary
 * @param power the order of two to construct boundary for
 *
 * @return the minimal @c 2^power aligned value closest to the @c x
 */
#define binalign(x, power) \
		binalign_bound(x, 0x1 << (power))

#ifndef __ASSEMBLER__

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
#define binalign_bound(x, bound) \
		__binalign_mask(x, (typeof(x)) (bound) - 1)

#else /* __ASSEMBLER__ */

#define binalign_bound(x, bound) \
		__binalign_mask(x, (bound) - 1)

#endif /* __ASSEMBLER__ */

#define __binalign_mask(x, mask)       (((x) + (mask)) & ~(mask))

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
#define binalign_check(x, power) \
		binalign_check_bound(x, 0x1 << (power))

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
#define binalign_check_bound(x, bound) \
		__binalign_check_mask(x, (typeof(x)) (bound) - 1)

#define __binalign_check_mask(x, mask)  ((x) & (mask) == 0)

#endif /* __ASSEMBLER__ */

#endif /* UTIL_BINALIGN_H_ */
