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

#define SCB_MEM_FAULT_STATUS        (ARM_M_SCB_BASE + 0x28)
# define SCB_MEM_FAULT_MMARVALID    (1 << 7)

#define SCB_BUS_FAULT_STATUS        (ARM_M_SCB_BASE + 0x29)
# define SCB_BUS_FAULT_BFARVALID    (1 << 7)

#define SCB_USAGE_FAULT_STATUS      (ARM_M_SCB_BASE + 0x2A)
#define SCB_HARD_FAULT_STATUS       (ARM_M_SCB_BASE + 0x2C)
#define SCB_MEM_FAULT_ADDRESS       (ARM_M_SCB_BASE + 0x34)
#define SCB_BUS_FAULT_ADDRESS       (ARM_M_SCB_BASE + 0x38)

#endif /* ARCH_ARM_ASM_ARM_M_REGS_H_ */
