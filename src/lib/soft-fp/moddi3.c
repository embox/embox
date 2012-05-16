/**
 * @file
 *
 * @brief
 *
 * @date 16.05.2012
 * @author Anton Bondarev
 */
#include <types.h>

extern int64_t __divdi3(int64_t num, int64_t den);
int64_t __moddi3(int64_t num, int64_t den) {
	int res;
	res = __divdi3(num,den);
	return num - res * den;
}
