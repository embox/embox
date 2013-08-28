/**
 * @file
 * @brief
 *
 * @date 16.05.12
 * @author Anton Bondarev
 * @author Ilia Vaprol
 */
#include <limits.h>
#include <stdint.h>

#define MASK_DWORD  (uint64_t)1 << ((WORD_BIT * 2) - 1)

uint64_t __udivdi3(uint64_t num, uint64_t den) {
	uint64_t result = 0;
	int steps;

	if (den == 0) {
		return 0;
	}

	steps = 0;
	result = 0;

	while (!(den & MASK_DWORD)) {
		den <<= 1;
		++steps;
	}

	do {
		result <<= 1;
		if (num >= den) {
			result |= 1;
			num -= den;
		}
		den >>= 1;
	} while (steps--);

	return result;
}
