/**
 * \file sparc_regs.S
 *
 * \brief some SPARC related registers
 *
 * \date Jun 4, 2009
 * \author Eldar Abusalimov
 */

#ifndef SPARC_REGS_H_
#define SPARC_REGS_H_

/*
 *  The SPARC v8 PSR layout.
 *
 *  ------------------------------------------------------------------------
 *  | impl  | vers  | icc   | resv  | EC | EF | PIL  | S | PS | ET |  CWP  |
 *  | 31-28 | 27-24 | 23-20 | 19-14 | 13 | 12 | 11-8 | 7 | 6  | 5  |  4-0  |
 *  ------------------------------------------------------------------------
 */

/* current window pointer     */
#define PSR_CWP     0x0000001f
/* enable traps field         */
#define PSR_ET      0x00000020
/* previous privilege level   */
#define PSR_PS      0x00000040
/* current privilege level    */
#define PSR_S       0x00000080
/* processor interrupt level  */
#define PSR_PIL     0x00000f00
/* enable floating point      */
#define PSR_EF      0x00001000
/* enable co-processor        */
#define PSR_EC      0x00002000
/* SuperSparcII little-endian */
#define PSR_LE      0x00008000
/* integer condition codes    */
#define PSR_ICC     0x00f00000
/* carry bit                  */
#define PSR_C       0x00100000
/* overflow bit               */
#define PSR_V       0x00200000
/* zero bit                   */
#define PSR_Z       0x00400000
/* negative bit               */
#define PSR_N       0x00800000
/* cpu-version field          */
#define PSR_VERS    0x0f000000
/* cpu-implementation field   */
#define PSR_IMPL    0xf0000000

#define PSR_BIT_TOGGLE(mask, scratch) \
	rd %psr, %scratch;                   \
	wr %scratch, %r_mask, %psr;

#define PSR_BIT_SET(r_mask, scratch) \
	mov %psr, %scratch;               \
	or %scratch, %r_mask, %scratch;   \
	mov %scratch, %psr;

#define PSR_BIT_CLEAR(r_mask, scratch) \
	mov %psr, %scratch;                 \
	andn %scratch, %r_mask, %scratch;   \
	mov %scratch, %psr;

#define PSR_BIT_COPY(r_src, r_mask, scratch0, scratch1) \
	mov  %psr, %scratch0;                 \
	andn %scratch0, %r_mask, %scratch0;   \
	and  %r_src, %r_mask, %scratch1;      \
	or   %scratch0, %scratch1, %scratch0; \
	mov  %scratch0, %psr;

/* check whether WIM[CWP] == 1 or not */
#define PSR_CWP_WIM_TEST(t_psr, t_wim, scratch) \
	and %t_psr, PSR_CWP, %scratch;  \
	srl %t_wim, %scratch, %scratch; \
	andcc %scratch, 0x1, %g0;

#define WRITE_PAUSE  nop; nop; nop;

/*
 *  The SPARC v8 TBR layout.
 *
 *  ------------------------
 *  | tba    | tt   | zero |
 *  | 31-12  | 11-4 | 3-0  |
 *  ------------------------
 */

/* zero (alignment)           */
#define TBR_ZERO    0x0000000f
/* trap type field            */
#define TBR_TT      0x00000ff0
/* trap base address          */
#define TBR_TBA     0xfffff000

#endif /* SPARC_REGS_H_ */
