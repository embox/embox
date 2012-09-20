/**
 * @file
 * @brief TODO
 *
 * @date Sep 20, 2012
 * @author Eldar Abusalimov
 */

#ifndef UTIL_BIT_H_
#define UTIL_BIT_H_

#include __impl_x(util/bit_impl.h)

/**
 * Find first (least significant) set bit.
 * @code
 *  bit_ffs(0x00000000ul) =  0
 *  bit_ffs(0x00000001ul) =  1
 *  bit_ffs(0xfffffffful) =  1
 *  bit_ffs(0x80000000ul) = 32
 * @endcode
 * @param x
 * @return
 *   One plus the index of the least significant set bit of @c x,
 *   or zero in case when @c x is zero too.
 */
extern int bit_ffs(unsigned long x);

/**
 * Find last (most significant) set bit.
 * @code
 *  bit_fls(0x00000000ul) =  0
 *  bit_fls(0x00000001ul) =  1
 *  bit_fls(0xfffffffful) = 32
 *  bit_fls(0x80000000ul) = 32
 * @endcode
 * @param x
 * @return
 *   One plus the index of the most significant set bit of @c x,
 *   or zero if @c x is zero.
 */
extern int bit_fls(unsigned long x);

#endif /* UTIL_BIT_H_ */
