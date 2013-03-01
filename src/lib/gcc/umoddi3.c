/**
 * @file
 * @brief
 *
 * @date 16.05.12
 * @author Anton Bondarev
 * @author Ilia Vaprol
 */

#include <stdint.h>

extern uint64_t __udivdi3(uint64_t num, uint64_t den);

uint64_t __umoddi3(uint64_t num, uint64_t den) {
	int steps;

	if (den == 0) {
		return 0;
	}

	steps = 0;

	while (!(den & 0x8000000000000000)) {
		den <<= 1;
		++steps;
	}

	do {
		if (num >= den) {
			num -= den;
		}
		den >>= 1;
	} while (steps--);

	return num;
}
