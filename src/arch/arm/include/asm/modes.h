/**
 * @file
 * @brief ARM modes
 *
 * @date 5.11.10
 * @author Anton Kozlov
 */

#ifndef ARM_MODES_H_
#define ARM_MODES_H_

#include <asm/psr.h>

#define ARM_MODE_FIQ 0x11
#define ARM_MODE_IRQ 0x12
#define ARM_MODE_SVC 0x13
#define ARM_MODE_ABT 0x17
#define ARM_MODE_UND 0x1B
#define ARM_MODE_USR 0x10
#define ARM_MODE_SYS 0x1f

/** disable IRQ, FIQ */
#define I_BIT 0x80
#define F_BIT 0x40

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

#endif /* ARM_MODES_H_ */
