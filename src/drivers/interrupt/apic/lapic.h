/**
 * @file
 *
 * @date 18.12.2012
 * @author Anton Bulychev
 */

#ifndef DRIVER_INTERRUPT_LAPIC_H_
#define DRIVER_INTERRUPT_LAPIC_H_

#include <stdint.h>
#include <drivers/interrupt/lapic/regs.h>
#include <drivers/interrupt/ioapic/regs.h>

#define LOCAL_APIC_DEF_ADDR	 0xFEE00000 /* Default local apic address */

#define LAPIC_ID             (LOCAL_APIC_DEF_ADDR + 0x020)
#define LAPIC_EOI            (LOCAL_APIC_DEF_ADDR + 0x0B0)
#define LAPIC_SIVR           (LOCAL_APIC_DEF_ADDR + 0x0F0)
#define LAPIC_ESR            (LOCAL_APIC_DEF_ADDR + 0x280)
#define LAPIC_ICR1           (LOCAL_APIC_DEF_ADDR + 0x300)
#define LAPIC_ICR2           (LOCAL_APIC_DEF_ADDR + 0x310)
#define LAPIC_LVT_TR         (LOCAL_APIC_DEF_ADDR + 0x320)
#define LAPIC_LVT_PCR        (LOCAL_APIC_DEF_ADDR + 0x340)
#define LAPIC_TIMER_ICR      (LOCAL_APIC_DEF_ADDR + 0x380)
#define LAPIC_TIMER_CCR      (LOCAL_APIC_DEF_ADDR + 0x390)
#define LAPIC_TIMER_DCR      (LOCAL_APIC_DEF_ADDR + 0x3E0)

#define LAPIC_DFR            (LOCAL_APIC_DEF_ADDR + 0x0E0)
#define LAPIC_LDR            (LOCAL_APIC_DEF_ADDR + 0x0D0)
#define LAPIC_LVT_LINT0      (LOCAL_APIC_DEF_ADDR + 0x350)
#define LAPIC_LVT_LINT1      (LOCAL_APIC_DEF_ADDR + 0x360)
#define LAPIC_TASKPRIOR      (LOCAL_APIC_DEF_ADDR + 0x80)

/* Delivery Mode */
#define LAPIC_ICR_DM_FIXED           (0 << 8)
#define LAPIC_ICR_DM_LOWPRIO         (1 << 8)
#define LAPIC_ICR_DM_SMI             (2 << 8)
#define LAPIC_ICR_DM_NMI             (4 << 8)
#define LAPIC_ICR_DM_INIT            (5 << 8)
#define LAPIC_ICR_DM_STARTUP         (6 << 8)

#define LAPIC_ICR_LOGICAL_DEST       (1 << 11)
#define LAPIC_ICR_DELIVERY_PENDING   (1 << 12)
#define LAPIC_ICR_INT_ASSERT         (1 << 14)
#define LAPIC_ICR_INIT_LEVELTRIG     (1 << 15)

/* Destination Shorthand */
#define LAPIC_ICR_DEST_FIELD         (0 << 18)
#define LAPIC_ICR_DEST_SELF          (1 << 18)
#define LAPIC_ICR_DEST_ALL           (2 << 18)
#define LAPIC_ICR_DEST_ALL_BUT_SELF  (3 << 18)

#define LAPIC_DISABLE    0x10000
#define LAPIC_SW_ENABLE  0x100
#define LAPIC_CPUFOCUS   0x200

static inline uint32_t lapic_id(void) {
	return lapic_read(LAPIC_ID) >> 24;
}

static inline uint32_t lapic_errstatus(void)
{
	lapic_write(LAPIC_ESR, 0);
	return lapic_read(LAPIC_ESR);
}

static inline void lapic_send_eoi(void) {
	lapic_write(LAPIC_EOI, 0);
}

extern int lapic_enable(void);
extern void lapic_send_ipi(unsigned int vector, unsigned int cpu, int type);
extern void lapic_send_init_ipi(uint32_t apic_id);
extern void lapic_send_startup_ipi(uint32_t apic_id, uint32_t trampoline);

#endif /* DRIVER_INTERRUPT_LAPIC_H_ */
