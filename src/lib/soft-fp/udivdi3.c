/**
 * @file
 * @brief
 *
 * @date 16.05.2012
 * @author Anton Bondarev
 */

#include <types.h>

uint64_t __udivdi3(uint64_t num, uint64_t den) {
	uint64_t result = 0;
	int steps = 0;
	int i;

	if (0 == den) {
		return 0;
	}
	while (0x8000000000000000 != (den & 0x8000000000000000)) {
		den <<= 1;
		steps++;
	}

	for (i = 0; i <= steps; i++) {
		result <<= 1;
		if (num >= den) {
			result += 1;
			num -= den;
		}
		den >>= 1;
	}
	return result;
}
