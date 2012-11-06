/**
 * @file
 * @brief PowerPC Microprocessor Family interrupt controller
 *
 * @date 06.11.12
 * @author Ilia Vaprol
 */

#include <types.h>

#include <drivers/irqctrl.h>

#include <embox/unit.h>
#include <prom/prom_printf.h>

EMBOX_UNIT_INIT(ppc_intc_init);

/**
 * Machine State Register (MSR)
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
static inline int32_t load_msr(void) { uint32_t reg; asm volatile ("mfmsr %0" : "=r" (reg)); return reg; }
static inline void store_msr(uint32_t reg) { asm volatile ("mtmsr %0" :: "r" (reg)); }

/**
 * Processor Version Register (PVR)
 * 0-15b  - Version
 * 16-31b - Revision
 */
static inline uint32_t load_pvr(void) { uint32_t reg; asm volatile ("mfpvr %0" : "=r" (reg)); return reg; }

/**
 * Execution and Contex Synchronizing Instruments
 */
static inline void sync(void) { asm volatile ("sync"); }
static inline void isync(void) { asm volatile ("isync"); }

extern void* memset(void *, int, size_t);
static int ppc_intc_init(void) {
	/* setup exception handling to big-endian mode */
	store_msr(load_msr() & ~(1 << 15));

	/* setup big-endian mode */
	store_msr(load_msr() & ~(1 << 31));
	sync();

	/* turn on external interrupt */
	store_msr(load_msr() | (1 << 16));

	/* allow user- and supervisor-level instructions */
	store_msr(load_msr() & ~(1 << 17));
	isync();

	/* setup exception prefix to 0x000n_nnnn */
	store_msr(load_msr() & ~(1 << 25));

//	memset((void*)(0x00000001), 0, 0x2FFF); /* try kill */

	return 0;
}

void irqctrl_enable(unsigned int interrupt_nr) {
}

void irqctrl_disable(unsigned int interrupt_nr) {
}

void irqctrl_clear(unsigned int interrupt_nr) {
}

void irqctrl_force(unsigned int interrupt_nr) {
}

void interrupt_handle(void) {
}

#if 0
#define PPC_INTC_BASE           0x48200000
#define PPC_INTC_RESERVED_0     0x00000000
#define PPC_INTC_SYS_RESET      0x00000100
#define PPC_INTC_MACH_CHECK     0x00000200
#define PPC_INTC_DSI            0x00000300
#define PPC_INTC_ISI            0x00000400
#define PPC_INTC_EXT_INTERRUPT  0x00000500
#define PPC_INTC_ALIGNMENT      0x00000600
#define PPC_INTC_PROGRAM        0x00000700
#define PPC_INTC_FP_UNAVAILABLE 0x00000800
#define PPC_INTC_DECREMENTER    0x00000
#define PPC_INTC_RESERVED_A
#define PPC_INTC_RESERVED_B
#define PPC_INTC_SYSTEM_CALL
#define PPC_INTC_TRACE
#define PPC_INTC_FP_ASSIST
#define PPC_INTC_RESERVED_F
#define PPC_INTC_SOFTWARE_EMULATION
#define PPC_INTC_ITLB_MISS
#define PPC_INTC_DTLB_MISS
#define PPC_INTC_ITLB_ERROR
#define PPC_INTC_DTLB_ERROR
#define PPC_INTC_RESERVED_15
#define PPC_INTC_RESERVED_16
#define PPC_INTC_RESERVED_17
#define PPC_INTC_RESERVED_18
#define PPC_INTC_RESERVED_19
#define PPC_INTC_RESERVED_1A
#define PPC_INTC_RESERVED_1B
#define PPC_INTC_DATA_BP
#define PPC_INTC_INSTRUCTION_BP
#define PPC_INTC_PERIPHERAL_BP
#define PPC_INTC_NMI
#endif

