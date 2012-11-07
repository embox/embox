/**
 * @file
 *
 * @date 17.10.12
 * @author Anton Bondarev
 * @author Ilia Vaprol
 */

#ifndef PPC_REGS_H_
#define PPC_REGS_H_

/**
 * Special Purpose Register (SPR) declarations
 */
#define	SPR_XER    1   /* fixed point exception register */
#define	SPR_LR     8   /* link register */
#define	SPR_CTR    9   /* count register */
#define	SPR_DSISR  18  /* DSI exception register */
#define	SPR_DAR    19  /* data access register */
#define	SPR_DEC    22  /* decrementer register */
#define	SPR_SRR0   26  /* save/restore register 0 */
#define	SPR_SRR1   27  /* save/restore register 1 */
#define	SPR_SPRG0  272 /* SPR general 0 */
#define	SPR_SPRG1  273 /* SPR general 1 */
#define	SPR_SPRG2  274 /* SPR general 2 */
#define	SPR_SPRG3  275 /* SPR general 3 */
#define	SPR_PVR    287 /* processor version register */

/**
 * Machine State Register (MSR) definations
 * 13b - Power management enable (POW)
 * 15b - Exception little-endian mode (ILE)
 * 16b - External interrupt enable (EE)
 * 17b - Privilege level (PR)
 * 18b - Floating-point available (FP)
 * 19b - Machine check enable (ME)
 * 20b - Floating-point exception mode 0 (FE0)
 * 21b - Single-step trace enable (SE)
 * 22b - Branch trace enable (BE)
 * 23b - Floating-point exception mode 1 (FE1)
 * 25b - Exception preﬁx (IP)
 * 26b - Instruction address translation (IR)
 * 27b - Data address translation (DR)
 * 30b - Recoverable exception (RI)
 * 31b - Little-endian mode enable (LE)
 */
#define	MSR_LE		0x00000001	/* little-endian mode enable */
#define	MSR_RI		0x00000002	/* recoverable exception */
#define	MSR_DR		0x00000010	/* data address translation */
#define	MSR_IR		0x00000020	/* instruction address translation */
#define	MSR_IP		0x00000040	/* exception prefix */
#define	MSR_FE1		0x00000100	/* floating-point exception mode 1 */
#define	MSR_BE		0x00000200	/* branch trace enable */
#define	MSR_SE		0x00000400	/* single-step trace enable */
#define	MSR_FE0		0x00000800	/* floating-point exception mode 0 */
#define	MSR_ME		0x00001000	/* machine check enable */
#define	MSR_FP		0x00002000	/* floating-point available */
#define	MSR_PR		0x00004000	/* privilege level (1:USR) */
#define	MSR_EE		0x00008000	/* external interrupt enable */
#define	MSR_ILE		0x00010000	/* exception little-endian mode (1:LE) */
#define	MSR_POW		0x00040000	/* power management enable */

#endif /* PPC_REGS_H_ */
