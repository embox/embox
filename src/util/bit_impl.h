/**
 * @file
 * @brief TODO
 *
 * @date Sep 20, 2012
 * @author Eldar Abusalimov
 */

#ifndef UTIL_BIT_IMPL_H_
#define UTIL_BIT_IMPL_H_

#define __bit_lsb_mask(n) ((0x1ul << (n)) - 1) /* n rightmost bits set. */
#define __bit_msb_mask(n) (~(~0x0ul >> (n)))   /* n leftmost bits set. */

/* Find first set. */
static inline int bit_ffs(unsigned long x) {
	int nr = 1;

	if (!x) {
		return 0;
	}

	/* Hope that compiler optimizes out the sizeof check. */
	if (sizeof(x) == 8 && !(x & __bit_lsb_mask(32))) {
		/* Suppress "shift count >= width of type" error in case
		 * when sizeof(x) is NOT 8, i.e. when it is a dead code anyway. */
		x >>= sizeof(x) * 8 / 2; /* guaranteed to be 32 */
		nr += 32;
	}
	if (!(x & __bit_lsb_mask(16))) {
		x >>= 16;
		nr += 16;
	}
	if (!(x & __bit_lsb_mask(8))) {
		x >>= 8;
		nr += 8;
	}
	if (!(x & __bit_lsb_mask(4))) {
		x >>= 4;
		nr += 4;
	}
	if (!(x & __bit_lsb_mask(2))) {
		x >>= 2;
		nr += 2;
	}
	nr += !(x & __bit_lsb_mask(1));

	return nr;
}

/* Find last set. */
static inline int bit_fls(unsigned long x) {
	int nr = sizeof(x) * 8;

	if (!x) {
		return 0;
	}

	if (sizeof(x) == 8 && !(x & __bit_msb_mask(32))) {
		x <<= sizeof(x) * 8 / 2; /* see bit_ffs */
		nr -= 32;
	}
	if (!(x & __bit_msb_mask(16))) {
		x <<= 16;
		nr -= 16;
	}
	if (!(x & __bit_msb_mask(8))) {
		x <<= 8;
		nr -= 8;
	}
	if (!(x & __bit_msb_mask(4))) {
		x <<= 4;
		nr -= 4;
	}
	if (!(x & __bit_msb_mask(2))) {
		x <<= 2;
		nr -= 2;
	}
	nr -= !(x & __bit_msb_mask(1));

	return nr;
}

#endif /* UTIL_BIT_IMPL_H_ */
