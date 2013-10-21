/**
 * @file
 *
 * @date 21.10.2013
 * @author Eldar Abusalimov
 */

#include <util/bitmap.h>
#include <util/bit.h>


unsigned int bitmap_find_set_bit(const unsigned long *bitmap,
		unsigned int size, unsigned int offset) {
	const unsigned long *p = bitmap + BITMAP_OFFSET(offset);
	unsigned long result = BITMAP_ROUND(offset);
	unsigned long tmp;

	if (offset >= size)
		return size;

	offset -= result;
	size -= result;
	tmp = *(p++);

	tmp &= (~0x0ul << offset);  /* mask out the beginning */

	while (size >= LONG_BIT) {
		if (tmp)
			goto found;
		result += LONG_BIT;
		size -= LONG_BIT;
		tmp = *(p++);
	}

	tmp &= (~0x0ul >> (LONG_BIT - size));  /* ...and the ending */
	if (!tmp)
		return result + size;

found:
	return result + bit_ctz(tmp);
}

