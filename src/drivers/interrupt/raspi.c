/**
 * @file
 * @brief Driver for Raspberry PI interrupt controller
 *
 * See BCM2835-ARM-Peripherals.pdf, 7 chapter for details.
 *
 * @date 06.07.15
 * @author Vita Loginova
 */

#include <assert.h>
#include <stdint.h>
#include <util/bit.h>

#include <kernel/irq.h>
#include <drivers/irqctrl.h>
#include <kernel/panic.h>

#define BCM2835_INTERRUPT_BASE 0x2000B200

#define BANK_CAPACITY 32

/**
 * Layout of the Interrupt registers.
 */
struct raspi_interrupt_regs {
	/**
	 * Shows which interrupt are pending.
	 */
    uint32_t irq_basic_pending;
    uint32_t irq_pending_1;
    uint32_t irq_pending_2;

    /**
     * TODO: not used yet.
     * There is no priority for any interrupt. If one interrupt is much more
     * important then all others it can be routed to the FIQ.
     */
    uint32_t fiq_control;

    /**
     * Writing a 1 to a bit will set the corresponding IRQ enable bit. All
     * other IRQ enable bits are unaffected. Only bits which are enabled
     * can be seen in the basic pending register.
     */
    uint32_t enable_irqs_1;
    uint32_t enable_irqs_2;
    uint32_t enable_basic_irqs;

    /**
     * Writing a 1 to a bit will clear the corresponding IRQ enable bit.
     * All other IRQ enable bits are unaffected.
     */
    uint32_t disable_irqs_1;
    uint32_t disable_irqs_2;
    uint32_t disable_basic_irqs;
};

static volatile struct raspi_interrupt_regs * const regs =
        (volatile struct raspi_interrupt_regs*)((int)BCM2835_INTERRUPT_BASE);

void irqctrl_enable(unsigned int interrupt_nr) {
	if (interrupt_nr < BANK_CAPACITY) {
		regs->enable_irqs_1 = 1 << interrupt_nr;
	} else if (interrupt_nr < BANK_CAPACITY*2) {
		regs->enable_irqs_2 = 1 << (interrupt_nr - BANK_CAPACITY);
	} else {
		regs->enable_basic_irqs = 1 << (interrupt_nr - BANK_CAPACITY*2);
	}
}

void irqctrl_disable(unsigned int interrupt_nr) {
	if (interrupt_nr < BANK_CAPACITY) {
		regs->disable_irqs_1 = 1 << interrupt_nr;
	} else if (interrupt_nr < BANK_CAPACITY*2) {
		regs->disable_irqs_2 = 1 << (interrupt_nr - BANK_CAPACITY);
	} else {
		regs->disable_basic_irqs = 1 << (interrupt_nr - BANK_CAPACITY*2);
	}
}

void irqctrl_clear(unsigned int interrupt_nr) {
	panic(__func__);
}

void irqctrl_force(unsigned int interrupt_nr) {
	panic(__func__);
}

static void __raspi__dispatch_bank(uint32_t pend_bank, int32_t add_to) {
	uint32_t bit;

	while(pend_bank) {
		bit = bit_ffs(pend_bank) - 1;
		pend_bank ^= (1 << bit);
		irq_dispatch(bit + add_to);
	}
}

void interrupt_handle(void) {
	assert(!critical_inside(CRITICAL_IRQ_LOCK));

	critical_enter(CRITICAL_IRQ_HANDLER);

	__raspi__dispatch_bank(regs->irq_pending_1, 0);
	__raspi__dispatch_bank(regs->irq_pending_2, (1 << 5));
	/*
	 * 31:21 bits are unused, 20:8 are used for speeding up interrupts
	 * processing by adding a number of 'normal' interrupt status bits there.
	 * It might be used in order to improve this driver later, but for now
	 * we apply a 0xFF mask to distinguish unique interrupt requests.
	 */
	__raspi__dispatch_bank(regs->irq_basic_pending & 0xFF, (2 << 5));

	critical_leave(CRITICAL_IRQ_HANDLER);

	critical_dispatch_pending();
}

void swi_handle(void) {
	panic(__func__);
}
