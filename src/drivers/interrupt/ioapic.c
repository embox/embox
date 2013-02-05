/**
 * @file
 * @brief
 *
 * @date 05.02.2013
 * @author Anton Bulychev
 */

#include <embox/unit.h>

#include <types.h>

#include <asm/io.h>
#include "i8259_regs.h"

#include <drivers/irqctrl.h>

#include <module/embox/driver/interrupt/lapic.h>

#define IOAPIC_DEF_ADDR	    0xFEC00000

#define IOREGSEL            (IOAPIC_DEF_ADDR + 0x00)
#define IOREGWIN            (IOAPIC_DEF_ADDR + 0x10)

#define IOAPIC_ID           0x0
#define IOAPIC_VERSION      0x1
#define IOAPIC_ARB          0x2
#define IOAPIC_REDIR_TABLE  0x10

#define APIC_ICR_INT_MASK   (1 << 16)

EMBOX_UNIT_INIT(ioapic_enable);

static inline uint32_t ioapic_read(uint8_t reg) {
	*((volatile uint32_t *)(IOREGSEL)) = reg;
	return *((volatile uint32_t *)(IOREGWIN));
}

static inline void ioapic_write(uint8_t reg, uint32_t val) {
	*((volatile uint32_t *)(IOREGSEL)) = reg;
	*((volatile uint32_t *)(IOREGWIN)) = val;
}


static inline void i8259_disable(void)
{
	outb(0xFF, 0xA1);
	outb(0xFF, 0x21);
	//inb(0x21);
}

/**
 * Initialize the PIC
 */
static int ioapic_enable(void) {
	static int inited = 0;
	int version, maxirq;
	if (1 == inited) {
		return 0;
	}
	inited = 1;

	version = ioapic_read(IOAPIC_VERSION);
	maxirq = (version & (0xFFUL << 16)) >> 16;

	for (int i = 0; i <= maxirq; i++) {
		irqctrl_disable(i);
	}

	/* I'm not sure that it is correct */
	outb(0x70, 0x22);
	outb(0x01, 0x23);

	return 0;
}

void apic_init(void) {
	lapic_enable();

	i8259_disable();
	ioapic_enable();
}

void irqctrl_enable(unsigned int irq) {
	uint32_t lo = ioapic_read(IOAPIC_REDIR_TABLE + irq * 2);
	lo &= ~APIC_ICR_INT_MASK;
	ioapic_write(IOAPIC_REDIR_TABLE + irq * 2, lo);
}

void irqctrl_disable(unsigned int irq) {
	u32_t lo = ioapic_read(IOAPIC_REDIR_TABLE + irq * 2);
	lo |= APIC_ICR_INT_MASK;
	ioapic_write(IOAPIC_REDIR_TABLE + irq * 2, lo);
}

void irqctrl_force(unsigned int irq) {
	// TODO Emm?.. -- Eldar
}

int i8259_irq_pending(unsigned int irq) {
	return 1;
}

/* Sends an EOI (end of interrupt) signal to the PICs. */
void i8259_send_eoi(unsigned int irq) {

}
