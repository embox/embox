/**
 * @file
 *
 * @date    19.06.2020
 * @author  Alexander Kalmuk
 */

#ifndef ARCH_ARM_ASM_ARM_M_REGS_H_
#define ARCH_ARM_ASM_ARM_M_REGS_H_

#define BIT(nr)                     (1U << (nr))

#define ARM_M_SCS_BASE              0xe000e000

/************ System Control Block registers (SCB) ************/

#define ARM_M_SCB_BASE              (ARM_M_SCS_BASE + 0xd00)

#define SCB_ICSR                    (ARM_M_SCB_BASE + 0x04)
# define SCB_ICSR_RETTOBASE         BIT(11)
# define SCB_ICSR_PENDSVSET         BIT(28)

#define SCB_VTOR                    (ARM_M_SCB_BASE + 0x08)
# define SCB_VTOR_IN_RAM            BIT(29)

/* Configuration Control Register */
#define SCB_CCR                     (ARM_M_SCB_BASE + 0x14)
# define SCB_CCR_IC                 BIT(17) /* I-cache enable */
# define SCB_CCR_DC                 BIT(16) /* D-cache enable */

/* System Handler Control and State Register */
#define SCB_SHCSR                   (ARM_M_SCB_BASE + 0x24)
# define SCB_SHCSR_MEMFAULTENA      BIT(16)

#define SCB_MEM_FAULT_STATUS        (ARM_M_SCB_BASE + 0x28)
# define SCB_MEM_FAULT_MMARVALID    BIT(7)

#define SCB_BUS_FAULT_STATUS        (ARM_M_SCB_BASE + 0x29)
# define SCB_BUS_FAULT_BFARVALID    BIT(7)

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
#define SCB_DCCSW                   (ARM_M_SCB_BASE + 0x26C)
# define SCB_DCCSW_WAY_POS          30
# define SCB_DCCSW_SET_POS          5

/************ MPU registers ************/
#define ARM_M_MPU_BASE              (ARM_M_SCS_BASE + 0xd90)

#define MPU_TYPE                    (ARM_M_MPU_BASE + 0x00)

#define MPU_CTRL                    (ARM_M_MPU_BASE + 0x04)
# define MPU_CTRL_PRIVDEFENA        BIT(2)
# define MPU_CTRL_HFNMIENA          BIT(1)
# define MPU_CTRL_ENABLE            BIT(0)

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

/*** Flash Patch and Breakpoint registers (FPB) ***/
#define ARM_M_FPB_BASE              0xe0002000
/* FlashPatch Control Register */
#define FP_CTRL                     (ARM_M_FPB_BASE + 0x00) 
/* FlashPatch Remap Register */
#define FP_REMAP                    (ARM_M_FPB_BASE + 0x04) 
/* FlashPatch Comparator Registers */
#define FP_COMP(n)                  (ARM_M_FPB_BASE + 0x08 + 0x04 * n)
/* FlashPatch Lock Access Register */
#define FP_LAR                      (ARM_M_FPB_BASE + 0xfb0)
# define FP_LAR_UNLOCK_KEY           0xc5acce55

/*** Debug Control Block Registers (DCB) ***/
#define ARM_M_DCB_BASE              0xe000edf0
#define DCB_DHCSR                   (ARM_M_DCB_BASE + 0x00) 
# define DCB_DCRSR_DBGKEY           (0xa05fUL << 16)
# define DCB_DCRSR_C_DEBUGEN        BIT(0)
#define DCB_DCRSR                   (ARM_M_DCB_BASE + 0x04) 
#define DCB_DCRDR                   (ARM_M_DCB_BASE + 0x08) 
#define DCB_DEMCR                   (ARM_M_DCB_BASE + 0x0c) 
# define DCB_DEMCR_MON_EN           BIT(16) /* Enable DebugMonitor Exception */
# define DCB_DEMCR_TRCENA           BIT(24)
#define DCB_DSCSR                   (ARM_M_DCB_BASE + 0x18) 

#define isb() \
	__asm__ volatile ("isb")

#define dsb() \
	__asm__ volatile ("dsb")

#endif /* ARCH_ARM_ASM_ARM_M_REGS_H_ */
