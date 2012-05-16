/**
 * @file
 *
 * @brief
 *
 * @date 16.05.2012
 * @author Anton Bondarev
 */
#include <types.h>

extern uint64_t __udivdi3(uint64_t num, uint64_t den);

int64_t __divdi3(int64_t num, int64_t den) {
	int minus = 0;
	int64_t v;

	if (num < 0) {
		num = -num;
		minus = 1;
	}
	if (den < 0) {
		den = -den;
		minus ^= 1;
	}

	v = __udivdi3(num, den);
	if (minus)
		v = -v;

	return v;
}
