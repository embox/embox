/**
 * @file
 * @brief
 *
 * @date 05.02.2013
 * @author Anton Bulychev
 */

#include <embox/unit.h>

#include <stdint.h>

#include <asm/io.h>

#include "i8259_regs.h"
#include "lapic.h"

#include <drivers/irqctrl.h>

#include <module/embox/driver/interrupt/lapic.h>

#define IOAPIC_DEF_ADDR	          0xFEC00000

#define IOREGSEL                  (IOAPIC_DEF_ADDR + 0x00)
#define IOREGWIN                  (IOAPIC_DEF_ADDR + 0x10)

#define IOAPIC_ID                 0x0
#define IOAPIC_VERSION            0x1
#define IOAPIC_ARB                0x2
#define IOAPIC_REDIR_TABLE        0x10

#define IOAPIC_ICR_VECTOR         0x000000FF
#define IOAPIC_ICR_INT_POLARITY   (1 << 13)
#define IOAPIC_ICR_TRIGGER        (1 << 15)
#define IOAPIC_ICR_INT_MASK       (1 << 16)

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
	outb(0xFF, PIC2_DATA);
	outb(0xFF, PIC1_DATA);
	//inb(PIC1_DATA);
}

/**
 * Initialize the IOAPIC
 */
static int ioapic_enable(void) {
	static int inited = 0;
	if (1 == inited) {
		return 0;
	}
	inited = 1;

#if 1
	/* I'm not sure that it is correct */
	outb(0x70, 0x22);
	outb(0x01, 0x23);
#endif

	return 0;
}

void apic_init(void) {
	i8259_disable();
	ioapic_enable();

	lapic_enable();
}

/* TODO: Rewrite it! */
static inline uint32_t irq_redir_low(unsigned int irq) {
	uint32_t val = 0;

	/* clear the polarity, trigger, mask and vector fields */
	val &= ~(IOAPIC_ICR_VECTOR | IOAPIC_ICR_INT_MASK |
			IOAPIC_ICR_TRIGGER | IOAPIC_ICR_INT_POLARITY);

	if (irq < 16) {
		/* ISA active-high */
		val &= ~IOAPIC_ICR_INT_POLARITY;
		/* ISA edge triggered */
		val &= ~IOAPIC_ICR_TRIGGER;
	}
	else {
		/* PCI active-low */
		val |= IOAPIC_ICR_INT_POLARITY;
		/* PCI level triggered */
		val |= IOAPIC_ICR_TRIGGER;
	}

	val |= (irq + 0x20);

	return val;
}

void irqctrl_enable(unsigned int irq) {
	uint32_t low, high;

	if (irq == 0) {
		/* LAPIC timer interrupt */
		return ;
	}

	low = irq_redir_low(irq);
	high = lapic_id() << 24;

	ioapic_write(IOAPIC_REDIR_TABLE + irq * 2 + 1, high);
	ioapic_write(IOAPIC_REDIR_TABLE + irq * 2, low);
}

void irqctrl_disable(unsigned int irq) {
	uint32_t low;

	if (irq == 0) {
		/* LAPIC timer interrupt */
		return ;
	}

	low = ioapic_read(IOAPIC_REDIR_TABLE + irq * 2);
	low |= IOAPIC_ICR_INT_MASK;
	ioapic_write(IOAPIC_REDIR_TABLE + irq * 2, low);
}

void irqctrl_force(unsigned int irq) {
	// TODO Emm?.. -- Eldar
}

int i8259_irq_pending(unsigned int irq) {
	// TODO Emm?.. -- Anton
	return 1;
}

void i8259_send_eoi(unsigned int irq) {
	//TODO: irq >= 16
	lapic_send_eoi();
}
