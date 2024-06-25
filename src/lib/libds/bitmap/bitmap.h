/**
 * @file
 *
 * @date 21.10.2013
 * @author Eldar Abusalimov
 */

#ifndef LIB_LIBDS_BITMAP_H_
#define LIB_LIBDS_BITMAP_H_

#include <limits.h> /* LONG_BIT */
#include <stdbool.h>
#include <string.h> /* memset */

#define BITMAP_DECL(name, nbits) unsigned long name[BITMAP_SIZE(nbits)]

#define BITMAP_SIZE(nbits)       (((nbits) + LONG_BIT - 1) / LONG_BIT)
#define BITMAP_OFFSET(bit)       ((bit) / LONG_BIT)
#define BITMAP_SHIFT(bit)        ((bit) % LONG_BIT)
#define BITMAP_MASK(bit)         (0x1ul << BITMAP_SHIFT(bit))

extern unsigned int __bitmap_find_bit(const unsigned long *bitmap,
    unsigned int nbits, unsigned int start, bool zero_bit);

static inline void bitmap_set_bit(unsigned long *bitmap, unsigned int bit) {
	bitmap[BITMAP_OFFSET(bit)] |= BITMAP_MASK(bit);
}

static inline void bitmap_clear_bit(unsigned long *bitmap, unsigned int bit) {
	bitmap[BITMAP_OFFSET(bit)] &= ~BITMAP_MASK(bit);
}

static inline void bitmap_toggle_bit(unsigned long *bitmap, unsigned int bit) {
	bitmap[BITMAP_OFFSET(bit)] ^= BITMAP_MASK(bit);
}

static inline unsigned int bitmap_test_bit(const unsigned long *bitmap,
    unsigned int bit) {
	return 0x1ul & (bitmap[BITMAP_OFFSET(bit)] >> BITMAP_SHIFT(bit));
}

static inline void bitmap_set_all(unsigned long *bitmap, unsigned int nbits) {
	memset(bitmap, ~0, BITMAP_SIZE(nbits) * sizeof(*bitmap));
}

static inline void bitmap_clear_all(unsigned long *bitmap, unsigned int nbits) {
	memset(bitmap, 0, BITMAP_SIZE(nbits) * sizeof(*bitmap));
}

static inline unsigned int bitmap_find_bit(const unsigned long *bitmap,
    unsigned int nbits, unsigned int start) {
	return __bitmap_find_bit(bitmap, nbits, start, false);
}

static inline unsigned int bitmap_find_zero_bit(const unsigned long *bitmap,
    unsigned int nbits, unsigned int start) {
	return __bitmap_find_bit(bitmap, nbits, start, true);
}

static inline unsigned int bitmap_find_first_bit(const unsigned long *bitmap,
    unsigned int nbits) {
	return bitmap_find_bit(bitmap, nbits, 0);
}

#endif /* LIB_LIBDS_BITMAP_H_ */
