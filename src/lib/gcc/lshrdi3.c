/**
 * @file
 *
 * @brief
 *
 * @date 18.05.2012
 * @author Anton Bondarev
 */


#include <stdint.h>
#include <limits.h>

typedef union {
	struct {
		uint32_t high;
		uint32_t low;
	} s;
	uint64_t ll;
} di_union;

/*64-bit logical shift right*/
uint64_t __lshrdi3(uint64_t u, unsigned int b) {
	di_union w, uu;
	int bm;

	if (b == 0)
		return u;

	uu.ll = u;

	bm = (sizeof(uint32_t) * CHAR_BIT) - b;

	if (bm <= 0) {
		w.s.high = 0;
		w.s.low = (uint32_t) uu.s.high >> -bm;
	} else {
		uint32_t carries = (uint32_t) uu.s.high << bm;
		w.s.high = (uint32_t) uu.s.high >> b;
		w.s.low = ((uint32_t) uu.s.low >> b) | carries;
	}

	return w.ll;
}
