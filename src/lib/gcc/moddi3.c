/**
 * @file
 * @brief
 *
 * @date 16.05.12
 * @author Anton Bondarev
 * @author Ilia Vaprol
 */

#include <stdint.h>

extern uint64_t __umoddi3(uint64_t num, uint64_t den);

int64_t __moddi3(int64_t num, int64_t den) {
	int64_t rem;
	int neg;

	num = num < 0 ? (neg = 1, -num) : (neg = 0, num);
	den = den < 0 ? (neg ^= 1, -den) : den;

	rem = __umoddi3(num, den);

	return neg ? -rem : rem;
}
