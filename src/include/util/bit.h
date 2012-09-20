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
 * Count trailing (least significant) zero bits.
 *
 * @param x
 *   Must not be 0.
 * @return
 *   The number of trailing zero bits in @c x.
 *
 * @code
 *  bit_ctz(0x00000001ul) =  0
 *  bit_ctz(0xfffffffful) =  0
 *  bit_ctz(0x80000000ul) = 31
 *  bit_ctz(0) // error, undefined
 * @endcode
 */
extern int bit_ctz(unsigned long x);

/**
 * Find first (least significant) set bit.
 *
 * @param x
 * @return
 *   One plus the index of the least significant set bit of @c x,
 *   or zero in case when @c x is zero too.
 */
static inline int bit_ffs(unsigned long x) {
	return x ? (bit_ctz(x) + 1) : 0;
}

/**
 * Count leading (most significant) zero bits.
 *
 * @param x
 *   Must not be 0.
 * @return
 *   The number of leading zero bits in @c x.
 *
 * @code
 *  bit_clz(0x00000001ul) = 31 // assuming the word is 32-bit width
 *  bit_clz(0xfffffffful) =  0
 *  bit_clz(0x80000000ul) =  0
 *  bit_clz(0) // error, undefined
 * @endcode
 */
extern int bit_clz(unsigned long x);

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
static inline int bit_fls(unsigned long x) {
	return x ? (sizeof(x) * 8 - bit_clz(x)) : 0;
}

#endif /* UTIL_BIT_H_ */
