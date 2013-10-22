/**
 * @file
 *
 * @date 21.10.2013
 * @author Eldar Abusalimov
 */

#include <util/bitmap.h>
#include <util/bit.h>


int bitmap_find_bit(const unsigned long *bitmap, int nbits, int offset) {
	const unsigned long *p = bitmap + BITMAP_OFFSET(offset);
	unsigned long result = BITMAP_ROUND(offset);
	unsigned long tmp;

	assert(nbits >= 0);
	assert(offset >= 0);

	if (offset >= nbits)
		return nbits;

	offset -= result;
	nbits -= result;
	tmp = *(p++);

	tmp &= (~0x0ul << offset);  /* mask out the beginning */

	while (nbits >= LONG_BIT) {
		if (tmp)
			goto found;
		result += LONG_BIT;
		nbits -= LONG_BIT;
		tmp = *(p++);
	}

	tmp &= (~0x0ul >> (LONG_BIT - nbits));  /* ...and the ending */
	if (!tmp)
		return result + nbits;

found:
	return result + bit_ctz(tmp);
}

