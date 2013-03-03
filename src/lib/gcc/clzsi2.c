/**
 * @file
 * @brief Returns the number of leading 0-bits
 *
 * @author  Anton Kozlov
 * @date    24.07.2012
 */

#include <stdint.h>

int32_t __clzsi2(int32_t a) {
	int i = 32;
	while (a) {
		a >>= 1;
		i--;
	}

	return i;
}
