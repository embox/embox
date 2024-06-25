/**
 * @file
 * @brief TODO
 *
 * @date 20.09.12
 * @author Eldar Abusalimov
 */

#ifndef LIB_LIBDS_BIT_H_
#define LIB_LIBDS_BIT_H_

#include <limits.h>

/**
 * Count trailing (least significant) zero bits.
 *
 * @param x
 *   Must not be 0.
 * @return
 *   The number of trailing zero bits in @c x.
 *
 * @code
 *  bit_ctz(0x00000001ul) = 0
 *  bit_ctz(0xfffffffful) = 0
 *  bit_ctz(0x80000000ul) = 31
 *  bit_ctz(0x00000000ul) = LONG_BIT
 * @endcode
 */
static inline int bit_ctz(unsigned long x) {
	return x ? __builtin_ctzl(x) : LONG_BIT;
}

/**
 * Find first (least significant) set bit.
 *
 * @param x
 * @return
 *   One plus the index of the least significant set bit of @c x,
 *   or zero in case when @c x is zero too.
 * 
 * @code
 *  bit_fls(0x00000001ul) = 1
 *  bit_fls(0xfffffffful) = 1
 *  bit_fls(0x80000000ul) = 32
 *  bit_fls(0x00000000ul) = 0
 * @endcode
 */
static inline int bit_ffs(unsigned long x) {
	return __builtin_ffsl(x);
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
 *  bit_clz(0x00000001ul) = LONG_BIT - 1
 *  bit_clz(0xfffffffful) = 0
 *  bit_clz(0x80000000ul) = 0
 *  bit_clz(0x00000000ul) = LONG_BIT
 * @endcode
 */
static inline int bit_clz(unsigned long x) {
	return x ? __builtin_clzl(x) : LONG_BIT;
}

/**
 * Find last (most significant) set bit.
 * 
 * @param x
 * @return
 *   One plus the index of the most significant set bit of @c x,
 *   or zero if @c x is zero.
 * 
 * @code
 *  bit_fls(0x00000001ul) =  1
 *  bit_fls(0xfffffffful) = 32
 *  bit_fls(0x80000000ul) = 32
 *  bit_fls(0x00000000ul) =  0
 * @endcode
 */
static inline int bit_fls(unsigned long x) {
	return LONG_BIT - bit_clz(x);
}

/**
 * For-like macro which iterates over set bits
 * of the given @code unsigned long x @endcode starting
 * from the least significant bit and using zero-based counting.
 *
 * @param bit
 *   @c int loop variable.
 * @param x
 */
#define bit_foreach(bit, x) __bit_foreach(bit, x, MACRO_GUARD(__bit))

#ifdef __clang__
// clang doesn't allow anonymous structs inside initializer in for loop
// because C standard is not very strict. Known in the Internet sinse 2013
// So, I decided temporarily to spoil the environment
// Kakadu
#define __bit_foreach(bit, _x, it)                  \
	struct __bit_iterator it = {.x = (_x), .b = 0}; \
	for (; it.x ? (bit = it.b = bit_ctz(it.x)), 1 : 0; it.x &= ~(0x1ul << it.b))
#else
#define __bit_foreach(bit, _x, it)                       \
	for (struct __bit_iterator it = {.x = (_x), .b = 0}; \
	     it.x ? (bit = it.b = bit_ctz(it.x)), 1 : 0; it.x &= ~(0x1ul << it.b))
#endif

struct __bit_iterator {
	unsigned long x;
	int b;
};

#endif /* LIB_LIBDS_BIT_H_ */
