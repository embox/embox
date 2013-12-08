/**
 * @file
 * @brief Defines register convention for arm architecture
 *
 * @date 19.09.10
 * @author Anton Bondarev
 * @author Ilia Vaprol
 */

#ifndef ARM_REGS_H_
#define ARM_REGS_H_

#ifdef __ASSEMBLER__

#define pc      r15

#define lr      r14

#define sp      r13

#endif /* __ASSEMBLER__ */

/**
 * c1, Control Register
 * read: MRC p15, 0, <Rd>, c1, c0, 0
 * write: MCR p15, 0, <Rd>, c1, c0, 0
 */
#define CR_M 0x0001 /* MMU enabled */
#define CR_A 0x0002 /* strict alignment fault checking enabled */
#define CR_C 0x0004 /* data caching enabled */
#define CR_Z 0x0800 /* program flow prediction enabled */
#define CR_I 0x1000 /* instruction caching enabled */

/**
 * c1, Auxiliary Control Register
 * read: MRC p15, 0, <Rd>, c1, c0, 1
 * write: MCR p15, 0, <Rd>, c1, c0, 1
 */
#define ACR_L2EN 0x2 /* L2 cache enabled */

#endif /* ARM_REGS_H_ */
