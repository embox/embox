/**
 * @file
 *
 * @date 21.10.2013
 * @author Eldar Abusalimov
 */

#include <limits.h>
#include <stdbool.h>

#include <lib/libds/bit.h>
#include <lib/libds/bitmap.h>

unsigned int __bitmap_find_bit(const unsigned long *bitmap, unsigned int nbits,
    unsigned int start, bool zero_bit) {
	const unsigned long *first;
	const unsigned long *last;
	const unsigned long *iter;
	unsigned long bits;

	if (start >= nbits) {
		return nbits;
	}

	first = bitmap + BITMAP_OFFSET(start);
	last = bitmap + BITMAP_OFFSET(nbits - 1);

	for (iter = first; iter <= last; iter++) {
		bits = *iter;
		if (zero_bit) {
			bits = ~bits;
		}
		if (iter == first) {
			bits &= ~0UL << BITMAP_SHIFT(start);
		}
		if (iter == last) {
			bits &= ~0UL >> (LONG_BIT - 1 - BITMAP_SHIFT(nbits - 1));
		}
		if (bits) {
			return LONG_BIT * (iter - bitmap) + bit_ctz(bits);
		}
	}

	return nbits;
}
