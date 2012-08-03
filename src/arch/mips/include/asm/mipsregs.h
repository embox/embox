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
#define CP0_DEBUG    23    /* Debug control and status */
#define CP0_DEPC     24    /* Program counter at last debug exception */
#define CP0_ERROREPC 30    /* Program counter at last error */
#define CP0_DESAVE   31    /* Debug handler scratchpad register */


#define MIPS_IRQN_TIMER     7

#define ST0_IRQ_MASK_OFFSET 0x8

#define ST0_IM     0x0000ff00
#define ST0_IP0   (1 << (ST0_IRQ_MASK_OFFSET + 0))
#define ST0_IP1   (1 << (ST0_IRQ_MASK_OFFSET + 1))
#define ST0_IP2   (1 << (ST0_IRQ_MASK_OFFSET + 2))    /* UART1 interrupt flag */
#define ST0_IP3   (1 << (ST0_IRQ_MASK_OFFSET + 3))    /* UART2 interrupt flag */
#define ST0_IP6   (1 << (ST0_IRQ_MASK_OFFSET + 6))    /* RTC interrupt flag */
#define ST0_IP7   (0x1 << (ST0_IRQ_MASK_OFFSET + 7))  /* timer interrupt flag */


#define ST0_IE       0x00000001
#define ST0_EXL      0x00000002
#define ST0_ERL      0x00000004
#define ST0_BEV      1 << 22    /* Boot exception vectors */


#define CAUSE_IP     1 << 7
#define CAUSE_IV     1 << 23
#define CAUSE_PCI    1 << 26   /* performance counter interrupt */
#define CAUSE_DC     1 << 27   /* stop counter */


#define __read_32bit_c0_register(source, sel)				\
({ int __res;								\
	if (sel == 0)							\
		__asm__ __volatile__(					\
			"mfc0\t%0, " #source "\n\t"			\
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
			"mtc0\t%z0, " #register "\n\t"			\
			: : "Jr" ((unsigned int)(value)));		\
	else								\
		__asm__ __volatile__(					\
			".set\tmips32\n\t"				\
			"mtc0\t%z0, " #register ", " #sel "\n\t"	\
			".set\tmips0"					\
			: : "Jr" ((unsigned int)(value)));		\
} while (0)


#define read_c0_status()	__read_32bit_c0_register($12, 0)
#define write_c0_status(val)	__write_32bit_c0_register($12, 0, val)

#define read_c0_cause()		__read_32bit_c0_register($13, 0)
#define write_c0_cause(val)	__write_32bit_c0_register($13, 0, val)

#define read_c0_compare()		__read_32bit_c0_register($11, 0)
#define write_c0_compare(val)	__write_32bit_c0_register($11, 0, val)

#define read_c0_count()		__read_32bit_c0_register($9, 0)
#define write_c0_count(val)	__write_32bit_c0_register($9, 0, val)

#define read_c0_intctl()		__read_32bit_c0_register($12, 1)
#define write_c0_intctl(val)	__write_32bit_c0_register($12, 1, val)

#define read_c0_ebase()		__read_32bit_c0_register($15, 1)
#define write_c0_ebase(val)	__write_32bit_c0_register($15, 1, val)


#endif /* MIPSREGS_H_ */
