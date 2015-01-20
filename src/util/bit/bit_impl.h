/**
 * @file
 * @brief TODO
 *
 * @date Sep 20, 2012
 * @author Eldar Abusalimov
 */

#ifndef UTIL_BIT_IMPL_H_
#define UTIL_BIT_IMPL_H_

#include <assert.h>

#define __bit_lsb_mask(n) ((0x1ul << (n)) - 1) /* n rightmost bits. */
#define __bit_msb_mask(n) (~(~0x0ul >> (n)))   /* n leftmost bits. */

/* Count trailing zeroes. */
static inline int bit_ctz(unsigned long x) {
	int nr = 0;
	int sh;

	assert(x);

	/* Hope that compiler optimizes out the sizeof check. */
	if (sizeof(x) == 8) {
		/* Suppress "shift count >= width of type" error in case
		 * when sizeof(x) is NOT 8, i.e. when it is a dead code anyway. */
		sh = !(x & __bit_lsb_mask(sizeof(x)*8/2)) << 5;
		nr += sh; x >>= sh;
	}

	sh = !(x & __bit_lsb_mask(1 << 4)) << 4; nr += sh; x >>= sh;
	sh = !(x & __bit_lsb_mask(1 << 3)) << 3; nr += sh; x >>= sh;
	sh = !(x & __bit_lsb_mask(1 << 2)) << 2; nr += sh; x >>= sh;
	sh = !(x & __bit_lsb_mask(1 << 1)) << 1; nr += sh; x >>= sh;
	sh = !(x & __bit_lsb_mask(1 << 0)) << 0; nr += sh;

	return nr;
}

/* Count leading zeroes. */
static inline int bit_clz(unsigned long x) {
	int nr = 0;
	int sh;

	assert(x);

	if (sizeof(x) == 8) {
		sh = !(x & __bit_msb_mask(sizeof(x)*8/2)) << 5; /* see bit_ffs */
		nr += sh; x <<= sh;
	}

	sh = !(x & __bit_msb_mask(1 << 4)) << 4; nr += sh; x <<= sh;
	sh = !(x & __bit_msb_mask(1 << 3)) << 3; nr += sh; x <<= sh;
	sh = !(x & __bit_msb_mask(1 << 2)) << 2; nr += sh; x <<= sh;
	sh = !(x & __bit_msb_mask(1 << 1)) << 1; nr += sh; x <<= sh;
	sh = !(x & __bit_msb_mask(1 << 0)) << 0; nr += sh;

	return nr;
}

#endif /* UTIL_BIT_IMPL_H_ */
