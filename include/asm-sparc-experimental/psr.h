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

#ifdef __ASSEMBLER__
/* some useful state register macros... */

#define PSR_BIT_TOGGLE(mask, scratch_psr) \
	rd %psr, %scratch_psr;       \
	wr %scratch_psr, mask, %psr;

#define PSR_BIT_TOGGLE_LONG(mask, scratch_psr, scratch_mask) \
	rd  %psr, %scratch_psr;                \
	set mask, %scratch_mask;               \
	wr  %scratch_psr, %scratch_mask, %psr;

#define PSR_BIT_SET(mask, scratch_psr) \
	rd  %psr, %scratch_psr;               \
	or  %scratch_psr, mask, %scratch_psr; \
	wr  %scratch_psr, %g0, %psr;

#define PSR_BIT_SET_LONG(mask, scratch_psr, scratch_mask) \
	rd  %psr, %scratch_psr;                        \
	set mask, %scratch_mask;                       \
	or  %scratch_psr, %scratch_mask, %scratch_psr; \
	wr  %scratch_psr, %g0, %psr;

#define PSR_BIT_CLEAR(mask, scratch_psr) \
	rd   %psr, %scratch_psr;               \
	andn %scratch_psr, mask, %scratch_psr; \
	wr   %scratch_psr, %g0, %psr;

#define PSR_BIT_CLEAR_LONG(mask, scratch_psr, scratch_mask) \
	rd   %psr, %scratch_psr;                       \
	set  mask, %scratch_mask;                      \
	andn %scratch_psr, %scratch_mask, %scratch_psr;\
	wr   %scratch_psr, %g0, %psr;

#endif /* __ASSEMBLER__ */

#endif /* SPARC_PSR_H_ */
