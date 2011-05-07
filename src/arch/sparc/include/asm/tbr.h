/**
 * @file
 * @brief SPARC v8 TBR (Table Base Register) layout.
 * @details
@verbatim
 +-------+------+--------+
 |  tba  |  tt  |  zero  |
 | 31-12 | 11-4 |   3-0  |
 +-------+------+--------+
@endverbatim
 *
 * @date 20.03.10
 * @author Alexander Batyukov
 */

#ifndef SPARC_TBR_H_
#define SPARC_TBR_H_

#define TBR_TBA 0xFFFFF000
#define TBR_TT	0x00000FF0

#ifndef __ASSEMBLER__

#include <types.h>

static inline uint32_t tbr_tba_get(void) {
	register unsigned tba;
	__asm__ __volatile__ (
		"rd %%tbr, %1\n\t"
		"and %1, %0, %1\n\t"
		:
		: "i" (TBR_TBA), "r" (tba)
		: "memory"
	);
	return tba;
}

static inline void tbr_tba_set(uint32_t tba) {
	__asm__ __volatile__ (
		"and %1, %0, %1\n\t"
		"wr %1, 0, %%tbr\n\t"
		" nop; nop; nop\n"
		:
		: "i" (TBR_TBA), "r" (tba)
		: "memory"
	);
}

#endif /* __ASSEMBLER__ */

#endif /* SPARC_TBR_H_ */
