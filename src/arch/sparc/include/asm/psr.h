/*
 * Derived from Linux (include/asm-sparc/psr.h).
 * Copyright (C) 1994 David S. Miller (davem@caip.rutgers.edu)
 */

/**
 * @file
 * @brief SPARC v8 PSR (Processor State Register) layout.
 * @details
@verbatim
 +-------+-------+-------+-------+----+----+------+---+----+----+-------+
 | impl  | vers  | icc   | resv  | EC | EF | PIL  | S | PS | ET |  CWP  |
 | 31-28 | 27-24 | 23-20 | 19-14 | 13 | 12 | 11-8 | 7 | 6  | 5  |  4-0  |
 +-------+-------+-------+-------+----+----+------+---+----+----+-------+
@endverbatim
 *
 * @date 04.06.2009
 * @author Eldar Abusalimov
 */

#ifndef SPARC_PSR_H_
#define SPARC_PSR_H_

#define PSR_CWP  0x0000001f /**< current window pointer     */
#define PSR_ET   0x00000020 /**< enable traps field         */
#define PSR_PS   0x00000040 /**< previous privilege level   */
#define PSR_S    0x00000080 /**< current privilege level    */
#define PSR_PIL  0x00000f00 /**< processor interrupt level  */
#define PSR_EF   0x00001000 /**< enable floating point      */
#define PSR_EC   0x00002000 /**< enable co-processor        */
#define PSR_LE   0x00008000 /**< SuperSparcII little-endian */
#define PSR_ICC  0x00f00000 /**< integer condition codes    */
#define PSR_C    0x00100000 /**< carry bit                  */
#define PSR_V    0x00200000 /**< overflow bit               */
#define PSR_Z    0x00400000 /**< zero bit                   */
#define PSR_N    0x00800000 /**< negative bit               */
#define PSR_VERS 0x0f000000 /**< cpu-version field          */
#define PSR_IMPL 0xf0000000 /**< cpu-implementation field   */

#ifndef __ASSEMBLER__

static inline unsigned int get_psr(void) {
	unsigned int psr;
	__asm__ __volatile__(
		"rd     %%psr, %0\n\t"
		"nop; nop; nop;\n\t"
		: "=r" (psr)
		: /* no inputs */
		: "memory"
	);

	return psr;
}

static inline void put_psr(unsigned int new_psr) {
	__asm__ __volatile__(
		"wr     %0, 0x0, %%psr\n\t"
		"nop; nop; nop;\n\t"
		: /* no outputs */
		: "r" (new_psr)
		: "memory", "cc"
	);
}

#endif /* __ASSEMBLER__ */

#endif /* SPARC_PSR_H_ */
