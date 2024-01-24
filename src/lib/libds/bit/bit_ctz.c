/**
 * @file
 * @brief TODO
 *
 * @date Sep 20, 2012
 * @author Eldar Abusalimov
 */

#include <assert.h>

#define __bit_lsb_mask(n) ((0x1ul << (n)) - 1) /* n rightmost bits. */
#define __bit_msb_mask(n) (~(~0x0ul >> (n)))   /* n leftmost bits. */

/* Count trailing zeroes. */
int bit_ctz(unsigned long x) {
	int nr;
	int sh;

	assert(x);

	nr = 0;

	/* Hope that compiler optimizes out the sizeof check. */
	if (sizeof(x) == 8) {
		/* Suppress "shift count >= width of type" error in case
		 * when sizeof(x) is NOT 8, i.e. when it is a dead code anyway. */
		sh = !(x & __bit_lsb_mask(sizeof(x) * 8 / 2)) << 5;
		nr += sh;
		x >>= sh;
	}

	/* clang-format off */
	sh = !(x & __bit_lsb_mask(1 << 4)) << 4; nr += sh; x >>= sh;
	sh = !(x & __bit_lsb_mask(1 << 3)) << 3; nr += sh; x >>= sh;
	sh = !(x & __bit_lsb_mask(1 << 2)) << 2; nr += sh; x >>= sh;
	sh = !(x & __bit_lsb_mask(1 << 1)) << 1; nr += sh; x >>= sh;
	sh = !(x & __bit_lsb_mask(1 << 0)) << 0; nr += sh;
	/* clang-format on */

	return nr;
}
