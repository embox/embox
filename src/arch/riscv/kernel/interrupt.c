/**
 * @file
 *
 * @brief
 *
 * @date 05.12.2019
 * @author Nastya Nizharadze
 */

#include <assert.h>
#include <asm/regs.h>
#include <kernel/irq.h>
#include <asm/ptrace.h>
#include <asm/interrupts.h>
#include <asm/entry.h>

#include <kernel/printk.h>

#include <embox/unit.h>

EMBOX_UNIT_INIT(riscv_interrupt_init);

#define CLEAN_IRQ_BIT (~(1 << 31))
#define PLIC_ADDR                  (uint32_t)OPTION_GET(NUMBER, plic_addr) //
#define CLAIM_COMPLETE_ADDR        (PLIC_ADDR + 0x200004U) /* offset for hart 0 claim/complere reg */
#define PRIORITY_THRESHOLD_ADDR    (PLIC_ADDR + 0x200000U)
#define INTERRUPT_ENABLE_REG_1     (PLIC_ADDR + 0x2000U)

void riscv_interrupt_handler(pt_regs_t *regs) {
	assert(!critical_inside(CRITICAL_IRQ_LOCK));

	critical_enter(CRITICAL_IRQ_HANDLER);
	{
		uint32_t pending;
		uint32_t *claim = (uint32_t *)CLAIM_COMPLETE_ADDR;
		uint32_t interrupt_id;
		uint32_t *interrupt_enable_reg;
		uint32_t enable_bit;

		pending = (read_csr(mcause)) & CLEAN_IRQ_BIT;
		interrupt_id = pending;

		if (pending == MACHINE_EXTERNAL_INTERRUPT) {
			/* the ID of the highest-priority pending interrupt */
			interrupt_id = *claim; 

			/* The current status of the interrupt enable bits in the PLIC core
			 * can be read from the enable array, organized as 2 words of 32 bits.
			 * The enable bit for interrupt ID N is stored in (N % 32) bit of 
			 * (N / 32) word. */
			interrupt_enable_reg = (uint32_t *)(INTERRUPT_ENABLE_REG_1 + ((interrupt_id / 32) * 4));
			enable_bit = 1U << (interrupt_id % 32);
			*interrupt_enable_reg = *interrupt_enable_reg & ~(enable_bit);
			interrupt_id += EXTERNAL_INTERRUPT_OFFSET;

			/* threshold 0x0C20_0000 masks all interrupt with ipl less than X,
			 * could try disable interrupts using this register*/
		}

		irqctrl_disable(pending);   //disable mie
		ipl_enable();               //enable mstatus.MIE
		irq_dispatch(interrupt_id); //call handler
		ipl_disable();              //disable mstatus.MIE
		irqctrl_enable(pending);    //enable mie

		if (pending == MACHINE_EXTERNAL_INTERRUPT) {
			*interrupt_enable_reg = *interrupt_enable_reg | (enable_bit);
			/* A hart signals it has completed executing an interrupt handler by
			 * writing the interrupt ID it received from the claim to the 
			 * claim/complete register. The PLIC does not check whether the
			 * completion ID is the same as the last claim ID for that target. */
			*claim = interrupt_id - EXTERNAL_INTERRUPT_OFFSET;
		}
	}
	critical_leave(CRITICAL_IRQ_HANDLER);
	critical_dispatch_pending();
}

static int riscv_interrupt_init(void) {
	uint32_t *priority_threshold = (uint32_t *) (PRIORITY_THRESHOLD_ADDR);
	*priority_threshold = 0;

	enable_external_interrupts();
	enable_interrupts();
	return 0;
}
