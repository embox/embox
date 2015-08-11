/* This file is derived from Newlib: http://sourceware.org/newlib/  */

/**
 * @file
 * @brief Implementation of #memset() function.
 *
 * @date 23.11.09
 * @author Eldar Abusalimov
 */

#include <string.h>
#include <stddef.h>
#include <stdint.h>

#include "inhibit_libcall.h"

#define BLOCK_SZ (sizeof(unsigned long))

/* Nonzero if X is not aligned on a "long" boundary.  */
#define unaligned(x)   ((unsigned long) x & (sizeof(unsigned long) - 1))

inhibit_loop_to_libcall
void *memset(void *addr_, int c, size_t n) {
	char *addr = addr_;
	unsigned long *aligned_addr;
	unsigned long buffer;
	unsigned int d = c & 0xff; /* To avoid sign extension.  */

	while (unaligned((uintptr_t) addr)) {
		if (n--) {
			*addr++ = (char) c;
		} else {
			return addr_;
		}
	}

	if (n >= BLOCK_SZ) {
		/* If we get this far, we know that n is large and addr is word-aligned. */
		aligned_addr = (unsigned long *) addr;

		/* Store D into each char sized location in BUFFER so that
		 we can set large blocks quickly.  */
		buffer = (d << 8) | d;
		for (int i = 16; i < BLOCK_SZ * 8; i <<= 1) {
			buffer |= (buffer << i);
		}

		/* Unroll the loop.  */
		for (; n >= BLOCK_SZ * 4; n -= BLOCK_SZ * 4) {
			*aligned_addr++ = buffer;
			*aligned_addr++ = buffer;
			*aligned_addr++ = buffer;
			*aligned_addr++ = buffer;
		}

		for (; n >= BLOCK_SZ; n -= BLOCK_SZ) {
			*aligned_addr++ = buffer;
		}
		/* Pick up the remainder with a bytewise loop.  */
		addr = (char *) aligned_addr;
	}

	while (n--) {
		*addr++ = (char) c;
	}

	return addr_;
}
