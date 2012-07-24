/**
 * @file
 *
 * @brief
 *
 * @date 26.06.2012
 * @author Anton Bondarev
 */

#ifndef MIPS_TRAPS_H_
#define MIPS_TRAPS_H_

#ifndef __ASSEMBLER__

typedef struct pt_regs {
	unsigned int reg[25];
	unsigned int gp; /* global pointer */
	unsigned int sp; /* stack pointer */
	unsigned int fp; /* frame pointer */
	unsigned int ra;
	unsigned int lo;
	unsigned int hi;
	unsigned int cp0_status;
	unsigned int pc;
}pt_regs_t;

#else
#define RVECENT(f,n) \
   b f; nop

#define XVECENT(f,bev) \
   b f     ;           \
   li k0,bev
#endif

#endif /* MIPS_TRAPS_H_ */
