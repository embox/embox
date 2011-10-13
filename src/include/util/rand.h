/**
 * @file
 * @brief Simple random generator implementation.
 *
 * @date Oct 12, 2011
 * @author Avdyukhin Dmitry
 */

#ifndef UTIL_RAND_H_
#define UTIL_RAND_H_

#include <types.h>
#include <stddef.h>

static unsigned long seed = 314567651;

static inline void srand(unsigned long new_seed) {
	seed = new_seed;
}

static inline unsigned long rand(void) {
	seed = (unsigned long)(seed * 16546134871 + 513585871) % (104814687);
	return seed;
}

#endif /* UTIL_RAND_H_ */
