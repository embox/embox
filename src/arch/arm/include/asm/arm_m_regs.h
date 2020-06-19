/**
 * @file
 *
 * @date    19.06.2020
 * @author  Alexander Kalmuk
 */

#ifndef ARCH_ARM_ASM_ARM_M_REGS_H_
#define ARCH_ARM_ASM_ARM_M_REGS_H_

/* System Control Block */
#define ARM_M_SCB_BASE              0xe000ed00
#define SCB_ICSR                    (ARM_M_SCB_BASE + 0x04)
# define SCB_ICSR_RETTOBASE         (1 << 11)
# define SCB_ICSR_PENDSVSET         (1 << 28)
#define SCB_VTOR                    (ARM_M_SCB_BASE + 0x08)
# define SCB_VTOR_IN_RAM            (1 << 29)

#endif /* ARCH_ARM_ASM_ARM_M_REGS_H_ */
