/**
 * @file
 *
 * @date 21.10.2013
 * @author Eldar Abusalimov
 */
#include <limits.h>

#include <util/bit.h>
#include <util/bitmap.h>

unsigned int bitmap_find_zero_bit(const unsigned long *bitmap,
    unsigned int nbits, unsigned int start) {
	const unsigned long *p, *pend;
	unsigned long tmp;

	if (start >= nbits) {
		return nbits;
	}

	p = bitmap + BITMAP_OFFSET(start);
	pend = bitmap + BITMAP_OFFSET(nbits);

	tmp = *p | ((1 << BITMAP_SHIFT(start)) - 1);

	while (p < pend) {
		if (~tmp) {
			goto found;
		}
		tmp = *(++p);
	}

	tmp |= ~(1 << BITMAP_SHIFT(nbits)) + 1;
	if (!~tmp) {
		return nbits;
	}

found:
	return LONG_BIT * (p - bitmap) + bit_ctz(~tmp);
}
