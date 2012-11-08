/**
 * @file
 * @brief PowerPC Microprocessor Family interrupt controller
 *
 * @date 06.11.12
 * @author Ilia Vaprol
 */

#include <asm/psr.h>
#include <asm/regs.h>

#include <drivers/irqctrl.h>

#include <embox/unit.h>

EMBOX_UNIT_INIT(ppc_intc_init);

static int ppc_intc_init(void) {
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

