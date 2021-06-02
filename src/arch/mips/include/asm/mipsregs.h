/**
 * @file
 *
 * @brief
 *
 * @date 04.07.2012
 * @author Anton Bondarev
 */

#ifndef MIPSREGS_H_
#define MIPSREGS_H_

#include <stdint.h>

/*
 *  Configure language
 */
#ifdef __ASSEMBLY__
#define _ULCAST_
#define _U64CAST_
#else
#define _ULCAST_ (unsigned long)
#define _U64CAST_ (u64)
#endif

/*
 * Coprocessor 0 registers.
 */
#define CP0_CONTEXT   4    /* The Pointer to an entry in (PTE) array */
#define CP0_BADVADDR  8    /* Virtual address of last exception */
#define CP0_COUNT     9    /* Processor cycle count */
#define CP0_COMPARE  11    /* Comparator value to generate timer IRQ7 */
#define CP0_STATUS   12    /* select 0 Processor status and control */
#define CP0_INTCTL   12    /* select 1 Interrupt control */
#define CP0_CAUSE    13    /* Cause of last exception */
#define CP0_EPC      14    /* Program counter at last exception */
#define CP0_PRID     15    /* Processor identification (read only) */
#define CP0_EBASE    15    /* select 1 exception base */
#define CP0_CONFIG   16    /* select 0 Configuration */
#define CP0_CONFIG1  16    /* select 1 configuration */
#define CP0_CONFIG2  16    /* select 2 Configuration */
#define CP0_CONFIG3  16    /* select 3 configuration */
#define CP0_CONFIG4  16    /* select 4 configuration */
#define CP0_CONFIG5  16    /* select 5 Configuration */
#define CP0_CONFIG6  16    /* select 6 configuration */
#define CP0_CONFIG7  16    /* select 7 configuration */
#define CP0_DEBUG    23    /* Debug control and status */
#define CP0_DEPC     24    /* Program counter at last debug exception */
#define CP0_ERROREPC 30    /* Program counter at last error */
#define CP0_DESAVE   31    /* Debug handler scratchpad register */


#define MIPS_IRQN_TIMER     7 /* timer interrupt number */

#define ST0_IRQ_MASK_OFFSET 0x8 /* interrupt mask offset in status and cause registers */
#define ST0_SOFTIRQ_NUM     0x2

#define ST0_IM     0x0000ff00
#define ST0_IP0   (1 << (ST0_IRQ_MASK_OFFSET + 0))    /* soft interrupt 0 */
#define ST0_IP1   (1 << (ST0_IRQ_MASK_OFFSET + 1))    /* soft interrupt 1 */
#define ST0_IP2   (1 << (ST0_IRQ_MASK_OFFSET + 2))    /* UART1 interrupt flag */
#define ST0_IP3   (1 << (ST0_IRQ_MASK_OFFSET + 3))    /* UART2 interrupt flag */
#define ST0_IP6   (1 << (ST0_IRQ_MASK_OFFSET + 6))    /* RTC interrupt flag */
#define ST0_IP7   (0x1 << (ST0_IRQ_MASK_OFFSET + MIPS_IRQN_TIMER))  /* timer interrupt flag */


#define ST0_IE       0x00000001                     /* interrupts enable mask */
#define ST0_EXL      0x00000002                     /* exception level mask */
#define ST0_ERL      0x00000004                     /* error level mask */
#define ST0_BEV      (1 << 22)                        /* Boot exception vectors */

#define CAUSE_IV     (1 << 23)   /* vectored interrupt table */
#define CAUSE_PCI    (1 << 26)   /* performance counter interrupt */
#define CAUSE_DC     (1 << 27)   /* stop counter */
#define CAUSE_TI     (1 << 30)   /* timer interrupt */
#define CAUSE_BD     (1 << 31)   /* branch delay */
#define CAUSE_IM     0x0000ff00 /* interrupts mask */

#define INTCTL_VS    0x000003E0

//#define CONFIG3_VEXIC (1 << 6)  /* Support for an external interrupt controller. */
//#define CONFIG3_VINT  (1 << 5)  /* Vectored interrupts implemented. */


/*
 * Bits in the coprocessor 0 config register.
 */
/* Generic bits.  */
#define CONF_CM_CACHABLE_NO_WA       0
#define CONF_CM_CACHABLE_WA          1
#define CONF_CM_UNCACHED             2
#define CONF_CM_CACHABLE_NONCOHERENT 3
#define CONF_CM_CACHABLE_CE          4
#define CONF_CM_CACHABLE_COW         5
#define CONF_CM_CACHABLE_CUW         6
#define CONF_CM_CACHABLE_ACCELERATED 7
#define CONF_CM_CMASK                7
#define CONF_BE                      (_ULCAST_(1) << 15)

/* Bits specific to the MIPS32/64 PRA.	*/
#define MIPS_CONF_VI                (_ULCAST_(1) <<  3)
#define MIPS_CONF_MT                (_ULCAST_(7) <<	 7)
#define MIPS_CONF_MT_TLB            (_ULCAST_(1) <<  7)
#define MIPS_CONF_MT_FTLB           (_ULCAST_(4) <<  7)
#define MIPS_CONF_AR                (_ULCAST_(7) << 10)
#define MIPS_CONF_AT                (_ULCAST_(3) << 13)
#define MIPS_CONF_IMPL              (_ULCAST_(0x1ff) << 16)
#define MIPS_CONF_M                 (_ULCAST_(1) << 31)

/*
 * Bits in the MIPS32/64 PRA coprocessor 0 config registers 1 and above.
 */
#define MIPS_CONF1_FP             (_ULCAST_(1) << 0)
#define MIPS_CONF1_EP             (_ULCAST_(1) << 1)
#define MIPS_CONF1_CA             (_ULCAST_(1) << 2)
#define MIPS_CONF1_WR             (_ULCAST_(1) << 3)
#define MIPS_CONF1_PC             (_ULCAST_(1) << 4)
#define MIPS_CONF1_MD             (_ULCAST_(1) << 5)
#define MIPS_CONF1_C2             (_ULCAST_(1) << 6)
#define MIPS_CONF1_DA_SHF          7
#define MIPS_CONF1_DA_SZ           3
#define MIPS_CONF1_DA             (_ULCAST_(7) << 7)
#define MIPS_CONF1_DL_SHF          10
#define MIPS_CONF1_DL_SZ           3
#define MIPS_CONF1_DL             (_ULCAST_(7) << 10)
#define MIPS_CONF1_DS_SHF          13
#define MIPS_CONF1_DS_SZ           3
#define MIPS_CONF1_DS             (_ULCAST_(7) << 13)
#define MIPS_CONF1_IA_SHF          16
#define MIPS_CONF1_IA_SZ           3
#define MIPS_CONF1_IA             (_ULCAST_(7) << 16)
#define MIPS_CONF1_IL_SHF          19
#define MIPS_CONF1_IL_SZ           3
#define MIPS_CONF1_IL             (_ULCAST_(7) << 19)
#define MIPS_CONF1_IS_SHF          22
#define MIPS_CONF1_IS_SZ           3
#define MIPS_CONF1_IS             (_ULCAST_(7) << 22)
#define MIPS_CONF1_TLBS_SHIFT     (25)
#define MIPS_CONF1_TLBS_SIZE      (6)
#define MIPS_CONF1_TLBS           (_ULCAST_(63) << MIPS_CONF1_TLBS_SHIFT)

#define MIPS_CONF2_SA_SHF   0
#define MIPS_CONF2_SA      (_ULCAST_(15) << 0)
#define MIPS_CONF2_SL_SHF   4
#define MIPS_CONF2_SL      (_ULCAST_(15) << 4)
#define MIPS_CONF2_SS_SHF   8
#define MIPS_CONF2_SS      (_ULCAST_(15) << 8)
#define MIPS_CONF2_L2B     (_ULCAST_(1) << 12)
#define MIPS_CONF2_SU      (_ULCAST_(15) << 12)
#define MIPS_CONF2_TA      (_ULCAST_(15) << 16)
#define MIPS_CONF2_TL      (_ULCAST_(15) << 20)
#define MIPS_CONF2_TS      (_ULCAST_(15) << 24)
#define MIPS_CONF2_TU      (_ULCAST_(7) << 28)

#define MIPS_CONF3_TL      (_ULCAST_(1) << 0)
#define MIPS_CONF3_SM      (_ULCAST_(1) << 1)
#define MIPS_CONF3_MT      (_ULCAST_(1) << 2)
#define MIPS_CONF3_CDMM    (_ULCAST_(1) << 3)
#define MIPS_CONF3_SP      (_ULCAST_(1) << 4)
#define MIPS_CONF3_VINT    (_ULCAST_(1) << 5)
#define MIPS_CONF3_VEIC    (_ULCAST_(1) << 6)
#define MIPS_CONF3_LPA     (_ULCAST_(1) << 7)
#define MIPS_CONF3_ITL     (_ULCAST_(1) << 8)
#define MIPS_CONF3_CTXTC   (_ULCAST_(1) <<	 9)
#define MIPS_CONF3_DSP     (_ULCAST_(1) << 10)
#define MIPS_CONF3_DSP2P   (_ULCAST_(1) << 11)
#define MIPS_CONF3_RXI     (_ULCAST_(1) << 12)
#define MIPS_CONF3_ULRI    (_ULCAST_(1) << 13)
#define MIPS_CONF3_ISA     (_ULCAST_(3) << 14)
#define MIPS_CONF3_ISA_OE  (_ULCAST_(1) << 16)
#define MIPS_CONF3_MCU     (_ULCAST_(1) << 17)
#define MIPS_CONF3_MMAR    (_ULCAST_(7) << 18)
#define MIPS_CONF3_IPLW    (_ULCAST_(3) << 21)
#define MIPS_CONF3_VZ      (_ULCAST_(1) << 23)
#define MIPS_CONF3_PW      (_ULCAST_(1) << 24)
#define MIPS_CONF3_SC      (_ULCAST_(1) << 25)
#define MIPS_CONF3_BI      (_ULCAST_(1) << 26)
#define MIPS_CONF3_BP      (_ULCAST_(1) << 27)
#define MIPS_CONF3_MSA     (_ULCAST_(1) << 28)
#define MIPS_CONF3_CMGCR   (_ULCAST_(1) << 29)
#define MIPS_CONF3_BPG     (_ULCAST_(1) << 30)

#define MIPS_CONF4_MMUSIZEEXT_SHIFT      (0)
#define MIPS_CONF4_MMUSIZEEXT            (_ULCAST_(255) << 0)
#define MIPS_CONF4_FTLBSETS_SHIFT        (0)
#define MIPS_CONF4_FTLBSETS              (_ULCAST_(15) << MIPS_CONF4_FTLBSETS_SHIFT)
#define MIPS_CONF4_FTLBWAYS_SHIFT        (4)
#define MIPS_CONF4_FTLBWAYS              (_ULCAST_(15) << MIPS_CONF4_FTLBWAYS_SHIFT)
#define MIPS_CONF4_FTLBPAGESIZE_SHIFT    (8)
#define MIPS_CONF4_FTLBPAGESIZE         (_ULCAST_(7) << MIPS_CONF4_FTLBPAGESIZE_SHIFT)
#define MIPS_CONF4_VFTLBPAGESIZE         (_ULCAST_(31) << MIPS_CONF4_FTLBPAGESIZE_SHIFT)
#define MIPS_CONF4_MMUEXTDEF             (_ULCAST_(3) << 14)
#define MIPS_CONF4_MMUEXTDEF_MMUSIZEEXT  (_ULCAST_(1) << 14)
#define MIPS_CONF4_MMUEXTDEF_FTLBSIZEEXT (_ULCAST_(2) << 14)
#define MIPS_CONF4_MMUEXTDEF_VTLBSIZEEXT (_ULCAST_(3) << 14)
#define MIPS_CONF4_KSCREXIST_SHIFT       (16)
#define MIPS_CONF4_KSCREXIST             (_ULCAST_(255) << MIPS_CONF4_KSCREXIST_SHIFT)
#define MIPS_CONF4_VTLBSIZEEXT_SHIFT     (24)
#define MIPS_CONF4_VTLBSIZEEXT           (_ULCAST_(15) << MIPS_CONF4_VTLBSIZEEXT_SHIFT)
#define MIPS_CONF4_AE                    (_ULCAST_(1) << 28)
#define MIPS_CONF4_IE                    (_ULCAST_(3) << 29)
#define MIPS_CONF4_TLBINV                (_ULCAST_(2) << 29)

#define MIPS_CONF5_NF        (_ULCAST_(1) << 0)
#define MIPS_CONF5_UFR       (_ULCAST_(1) << 2)
#define MIPS_CONF5_MRP       (_ULCAST_(1) << 3)
#define MIPS_CONF5_LLB       (_ULCAST_(1) << 4)
#define MIPS_CONF5_MVH       (_ULCAST_(1) << 5)
#define MIPS_CONF5_VP        (_ULCAST_(1) << 7)
#define MIPS_CONF5_SBRI      (_ULCAST_(1) << 6)
#define MIPS_CONF5_FRE       (_ULCAST_(1) << 8)
#define MIPS_CONF5_UFE       (_ULCAST_(1) << 9)
#define MIPS_CONF5_L2C       (_ULCAST_(1) << 10)
#define MIPS_CONF5_CA2       (_ULCAST_(1) << 14)
#define MIPS_CONF5_MI        (_ULCAST_(1) << 17)
#define MIPS_CONF5_CRCP      (_ULCAST_(1) << 18)
#define MIPS_CONF5_MSAEN     (_ULCAST_(1) << 27)
#define MIPS_CONF5_EVA       (_ULCAST_(1) << 28)
#define MIPS_CONF5_CV        (_ULCAST_(1) << 29)
#define MIPS_CONF5_K         (_ULCAST_(1) << 3


/* helpers for access to MIPS co-processor 0 registers */
#define __read_32bit_c0_register(source, sel)				\
({ int __res;								\
	if (sel == 0)							\
		__asm__ __volatile__(					\
			"mfc0\t%0, " #source "\n\t  ehb"			\
			: "=r" (__res));				\
	else								\
		__asm__ __volatile__(					\
			".set\tmips32\n\t"				\
			"mfc0\t%0, " #source ", " #sel "\n\t"		\
			".set\tmips0\n\t"				\
			: "=r" (__res));				\
	__res;								\
})

#define __write_32bit_c0_register(register, sel, value)			\
do {									\
	if (sel == 0)							\
		__asm__ __volatile__(					\
			"mtc0\t%z0, " #register "\n\t ehb"			\
			: : "Jr" ((unsigned int)(value)));		\
	else								\
		__asm__ __volatile__(					\
			".set\tmips32\n\t"				\
			"mtc0\t%z0, " #register ", " #sel "\n\t"	\
			".set\tmips0"					\
			: : "Jr" ((unsigned int)(value)));		\
} while (0)


/* read/write mips status register */
#define mips_read_c0_status()	__read_32bit_c0_register($12, 0)
#define mips_write_c0_status(val)	__write_32bit_c0_register($12, 0, val)

/* read/write mips cause register */
#define mips_read_c0_cause()		__read_32bit_c0_register($13, 0)
#define mips_write_c0_cause(val)	__write_32bit_c0_register($13, 0, val)

/* read/write mips compare register */
#define mips_read_c0_compare()		__read_32bit_c0_register($11, 0)
#define mips_write_c0_compare(val)	__write_32bit_c0_register($11, 0, val)

/* read/write mips count register */
#define mips_read_c0_count()		__read_32bit_c0_register($9, 0)
#define mips_write_c0_count(val)	__write_32bit_c0_register($9, 0, val)

/* read/write mips interrupt control register */
#define mips_read_c0_intctl()		__read_32bit_c0_register($12, 1)
#define mips_write_c0_intctl(val)	__write_32bit_c0_register($12, 1, val)

/* read/write mips exception base register */
#define mips_read_c0_ebase()		__read_32bit_c0_register($15, 1)
#define mips_write_c0_ebase(val)	__write_32bit_c0_register($15, 1, val)

/* read/write mips config register */
#define mips_read_c0_config()		__read_32bit_c0_register($16, 0)
#define mips_write_c0_config(val)	__write_32bit_c0_register($16, 0, val)

/* read/write mips config1 register */
#define mips_read_c0_config1()		__read_32bit_c0_register($16, 1)
#define mips_write_c0_config1(val)	__write_32bit_c0_register($16, 1, val)

/* read/write mips config2 register */
#define mips_read_c0_config2()		__read_32bit_c0_register($16, 2)
#define mips_write_c0_config2(val)	__write_32bit_c0_register($16, 2, val)

/* read/write mips config3 register */
#define mips_read_c0_config3()		__read_32bit_c0_register($16, 3)
#define mips_write_c0_config3(val)	__write_32bit_c0_register($16, 3, val)

/* read/write mips config4 register */
#define mips_read_c0_config4()		__read_32bit_c0_register($16, 4)
#define mips_write_c0_config4(val)	__write_32bit_c0_register($16, 4, val)

/* read/write mips config5 register */
#define mips_read_c0_config5()		__read_32bit_c0_register($16, 5)
#define mips_write_c0_config5(val)	__write_32bit_c0_register($16, 5, val)

/* read/write mips config6 register */
#define mips_read_c0_config6()		__read_32bit_c0_register($16, 6)
#define mips_write_c0_config6(val)	__write_32bit_c0_register($16, 6, val)

/* read/write mips config7 register */
#define mips_read_c0_config7()		__read_32bit_c0_register($16, 7)
#define mips_write_c0_config7(val)	__write_32bit_c0_register($16, 7, val)

#ifndef __ASSEMBLER__
static inline unsigned int mips_change_c0_config(unsigned int change, unsigned int val) {
	unsigned int res, new_val;

	res = mips_read_c0_config();
	new_val = res & ~change;
	new_val |= (val & change);
	mips_write_c0_config(new_val);

	return res;
}
#endif /* __ASSEMBLY__ */

#endif /* MIPSREGS_H_ */
