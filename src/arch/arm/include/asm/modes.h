/**
 * @file
 * @brief ARM modes
 *
 * @date 5.11.2010
 * @author Anton Kozlov
 */

#ifndef ARM_MODES_H_
#define ARM_MODES_H_

#ifdef __ASSEMBLER__

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

#endif /* __ASSEMBLER__ */

#endif /* ARM_MODES_H_ */
