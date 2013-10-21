/**
 * @file
 * @brief Simple random generator implementation.
 * See linear random generator for algoritm information.
 *
 * @date 12.10.11
 * @author Avdyukhin Dmitry
 */

#include <stdlib.h>

static unsigned int seed = 314567651;

void srand(unsigned int new_seed) {
	seed = new_seed;
}

int rand(void) {
	seed = (unsigned int)(seed * 16546134871 + 513585871) % (204814687);
	return (int)seed >> 1;
}

int rand_r(unsigned int *seedp) {
	*seedp = (unsigned int)(*seedp * 16546134871 + 513585871) % (204814687);
	return (int)(*seedp) >> 1;
}
