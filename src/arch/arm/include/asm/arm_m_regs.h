/**
 * @file
 *
 * @date    19.06.2020
 * @author  Alexander Kalmuk
 */

#ifndef ARCH_ARM_ASM_ARM_M_REGS_H_
#define ARCH_ARM_ASM_ARM_M_REGS_H_

#define ARM_M_SCS_BASE              0xe000e000

/************ System Control Block registers (SCB) ************/

#define ARM_M_SCB_BASE              (ARM_M_SCS_BASE + 0xd00)

#define SCB_ICSR                    (ARM_M_SCB_BASE + 0x04)
# define SCB_ICSR_RETTOBASE         (1 << 11)
# define SCB_ICSR_PENDSVSET         (1 << 28)

#define SCB_VTOR                    (ARM_M_SCB_BASE + 0x08)
# define SCB_VTOR_IN_RAM            (1 << 29)

/* Configuration Control Register */
#define SCB_CCR                     (ARM_M_SCB_BASE + 0x14)
# define SCB_CCR_IC                 (1 << 17) /* I-cache enable */
# define SCB_CCR_DC                 (1 << 16) /* D-cache enable */

/* System Handler Control and State Register */
#define SCB_SHCSR                   (ARM_M_SCB_BASE + 0x24)
# define SCB_SHCSR_MEMFAULTENA      (1 << 16)

#define SCB_MEM_FAULT_STATUS        (ARM_M_SCB_BASE + 0x28)
# define SCB_MEM_FAULT_MMARVALID    (1 << 7)

#define SCB_BUS_FAULT_STATUS        (ARM_M_SCB_BASE + 0x29)
# define SCB_BUS_FAULT_BFARVALID    (1 << 7)

#define SCB_USAGE_FAULT_STATUS      (ARM_M_SCB_BASE + 0x2A)
#define SCB_HARD_FAULT_STATUS       (ARM_M_SCB_BASE + 0x2C)
#define SCB_MEM_FAULT_ADDRESS       (ARM_M_SCB_BASE + 0x34)
#define SCB_BUS_FAULT_ADDRESS       (ARM_M_SCB_BASE + 0x38)

/* Cache Size ID Register */
#define SCB_CCSIDR                  (ARM_M_SCB_BASE + 0x80)
# define SCB_CCSIDR_SETS_POS        13
# define SCB_CCSIDR_SETS_MASK       0x7fff
# define SCB_CCSIDR_WAYS_POS        3
# define SCB_CCSIDR_WAYS_MASK       0x3ff
/* Cache Size Selection Register */
#define SCB_CSSELR                  (ARM_M_SCB_BASE + 0x84)
/* I-Cache Invalidate All to PoU */
#define SCB_ICIALLU                 (ARM_M_SCB_BASE + 0x250)
/* D-Cache Invalidate by Set-way */
#define SCB_DCISW                   (ARM_M_SCB_BASE + 0x260)
# define SCB_DCISW_WAY_POS          30
# define SCB_DCISW_SET_POS          5

/************ MPU registers ************/
#define ARM_M_MPU_BASE              (ARM_M_SCS_BASE + 0xd90)

#define MPU_TYPE                    (ARM_M_MPU_BASE + 0x00)

#define MPU_CTRL                    (ARM_M_MPU_BASE + 0x04)
# define MPU_CTRL_PRIVDEFENA        (1 << 2)
# define MPU_CTRL_HFNMIENA          (1 << 1)
# define MPU_CTRL_ENABLE            (1 << 0)

#define MPU_RNR                     (ARM_M_MPU_BASE + 0x08)

#define MPU_RBAR                    (ARM_M_MPU_BASE + 0x0C)

#define MPU_RASR                    (ARM_M_MPU_BASE + 0x10)
# define MPU_RASR_XN_POS            28
# define MPU_RASR_AP_POS            24
# define MPU_RASR_TEX_POS           19
# define MPU_RASR_S_POS             18
# define MPU_RASR_C_POS             17
# define MPU_RASR_B_POS             16
# define MPU_RASR_SRD_POS           8
# define MPU_RASR_SIZE_POS          1
# define MPU_RASR_ENABLE_POS        0

#define isb() \
	__asm__ volatile ("isb")

#define dsb() \
	__asm__ volatile ("dsb")

#endif /* ARCH_ARM_ASM_ARM_M_REGS_H_ */
