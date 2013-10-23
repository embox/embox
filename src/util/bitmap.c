/**
 * @file
 *
 * @date 21.10.2013
 * @author Eldar Abusalimov
 */

#include <util/bitmap.h>
#include <util/bit.h>


unsigned int bitmap_find_bit(const unsigned long *bitmap,
		unsigned int nbits, unsigned int start) {
	const unsigned long *p = bitmap + BITMAP_OFFSET(start);  /* start word */
	unsigned int shift = BITMAP_SHIFT(start);  /* within the start word */
	unsigned int result = start - shift;  /* LONG_BIT-aligned down start */
	unsigned long tmp;

	if (start >= nbits)
		return nbits;

	nbits -= result;
	tmp = *(p++) & (~0x0ul << shift);  /* mask out the beginning */

	while (nbits > LONG_BIT) {
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

