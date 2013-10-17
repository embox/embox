/* This file is derived from Newlib: http://sourceware.org/newlib/  */

/**
 * @file
 * @brief Implementation of #memcpy() function.
 *
 * @date 20.02.13
 * @author Eldar Abusalimov
 */

#include <string.h>
#include <stddef.h>
#include <stdint.h>

/* How many bytes are copied each iteration of the word copy loop.  */
#define BLOCK_SZ (sizeof(long))

/* Nonzero if either X or Y is not aligned on a "long" boundary.  */
#define unaligned(x, y) \
  (((long) x | (long) y) & (sizeof(long) - 1))

void *memcpy(void *dst_, const void *src_, size_t n) {
	char *dst = dst_;
	long *aligned_dst;
	const char *src = src_;
	const long *aligned_src;

	/* If the size is small, or either SRC or DST is unaligned,
	 then punt into the byte copy loop.  This should be rare.  */
	if (n >= BLOCK_SZ * 4 && !unaligned((intptr_t) src, (intptr_t) dst)) {
		aligned_dst = (long *) dst;
		aligned_src = (long *) src;

		/* Copy 4X long words at a time if possible.  */
		for (; n >= BLOCK_SZ * 4; n -= BLOCK_SZ * 4) {
			*aligned_dst++ = *aligned_src++;
			*aligned_dst++ = *aligned_src++;
			*aligned_dst++ = *aligned_src++;
			*aligned_dst++ = *aligned_src++;
		}

		/* Copy one long word at a time if possible.  */
		for (; n >= BLOCK_SZ; n -= BLOCK_SZ) {
			*aligned_dst++ = *aligned_src++;
		}

		/* Pick up any residual with a byte copier.  */
		dst = (char *) aligned_dst;
		src = (char *) aligned_src;
	}

	while (n--) {
		*dst++ = *src++;
	}

	return dst_;
}
