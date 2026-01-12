/**
 * @file
 * @brief Implementation of ffs() function.
 *
 * @date 15.03.20
 * @author Byeonggon Lee
 */

#include <strings.h>

int ffs(int in) {
	for (int i = 0; i < sizeof(int) * 8; ++i) {
		if (in & (1 << i)) {
			return i + 1;
		}
	}
	return 0;
}
