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

uint64_t __umoddi3(uint64_t num, uint64_t den) {
	uint64_t res;

	res = __udivdi3(num,den);

	return num - res * den;
}
