/**
 * @file
 * @brief OMAP35x interrupt controller driver
 *
 * @date 15.10.12
 * @author Ilia Vaprol
 */

#include <assert.h>
#include <types.h>

#include <kernel/critical.h>
#include <hal/reg.h>
#include <hal/ipl.h>
#include <drivers/irqctrl.h>

#include <kernel/irq.h>
#include <embox/unit.h>

EMBOX_UNIT_INIT(omap3_intc_init);

/**
 * Specefic constants (n = 0 to 2, m = 0 to 95)
 */
#define OMAP35X_INTC_BASE           0x48200000
#define OMAP35X_INTC_REVISION       (OMAP35X_INTC_BASE + 0x00000000)
#define OMAP35X_INTC_SYSCONFIG      (OMAP35X_INTC_BASE + 0x00000010)
#define OMAP35X_INTC_SYSSTATUS      (OMAP35X_INTC_BASE + 0x00000014)
#define OMAP35X_INTC_SIR_IRQ        (OMAP35X_INTC_BASE + 0x00000040)
#define OMAP35X_INTC_SIR_FIQ        (OMAP35X_INTC_BASE + 0x00000044)
#define OMAP35X_INTC_CONTROL        (OMAP35X_INTC_BASE + 0x00000048)
#define OMAP35X_INTC_PROTECTION     (OMAP35X_INTC_BASE + 0x0000004C)
#define OMAP35X_INTC_IDLE           (OMAP35X_INTC_BASE + 0x00000050)
#define OMAP35X_INTC_IRQ_PRIORITY   (OMAP35X_INTC_BASE + 0x00000060)
#define OMAP35X_INTC_FIQ_PRIORITY   (OMAP35X_INTC_BASE + 0x00000064)
#define OMAP35X_INTC_THRESHOLD      (OMAP35X_INTC_BASE + 0x00000068)
#define OMAP35X_INTC_ITR(n)         (OMAP35X_INTC_BASE + 0x00000080 + (n) * 0x20)
#define OMAP35X_INTC_MIR(n)         (OMAP35X_INTC_BASE + 0x00000084 + (n) * 0x20)
#define OMAP35X_INTC_MIR_CLEAR(n)   (OMAP35X_INTC_BASE + 0x00000088 + (n) * 0x20)
#define OMAP35X_INTC_MIR_SET(n)     (OMAP35X_INTC_BASE + 0x0000008C + (n) * 0x20)
#define OMAP35X_INTC_ISR_SET(n)     (OMAP35X_INTC_BASE + 0x00000090 + (n) * 0x20)
#define OMAP35X_INTC_ISR_CLEAR(n)   (OMAP35X_INTC_BASE + 0x00000094 + (n) * 0x20)
#define OMAP35X_INTC_PENDING_IRQ(n) (OMAP35X_INTC_BASE + 0x00000098 + (n) * 0x20)
#define OMAP35X_INTC_PENDING_FIQ(n) (OMAP35X_INTC_BASE + 0x0000009C + (n) * 0x20)
#define OMAP35X_INTC_ILR(m)         (OMAP35X_INTC_BASE + 0x00000100 + (m) * 0x4)

extern char trap_table_start, trap_table_end;

uint32_t load_sctlr(void) { uint32_t reg; asm volatile ("mrc  p15, 0, %0, c1, c0, 0" : "=r" (reg)); return reg; }
void store_sctlr(uint32_t reg) { asm volatile ("mcr  p15, 0, %0, c1, c0, 0" :: "r" (reg)); }

uint32_t load_snvbar(void) { uint32_t reg; asm volatile ("mrc  p15, 0, %0, c12, c0, 0" : "=r" (reg)); return reg; }
void store_snvbar(uint32_t reg) { asm volatile ("mcr  p15, 0, %0, c12, c0, 0" :: "r" (reg)); }

 void memcpy32(volatile uint32_t *dest, void *src, size_t len) {
	size_t lenw = (size_t) ((len & (~3)) >> 2);
	volatile uint32_t *srcw = (uint32_t*) ((uint32_t) (src) & (~3));

	while (lenw--) {
		*dest++ = *srcw++;
	}
	if (len & (~3)) {
		*dest++ = *srcw++;
	}
}
#include <prom/prom_printf.h>
#include <string.h>
static int omap3_intc_init(void) {
	memcpy32((uint32_t *)0x4020f800, &trap_table_start, (size_t)&trap_table_end - (size_t)&trap_table_start);
	__asm__ __volatile__ ("swi 15");
#if 0
	store_snvbar((uint32_t)&trap_table_start);
	prom_printf("snvbar: 0x%x\n", load_snvbar());
	memcpy32((uint32_t *)0x0000, &trap_table_start, (size_t)&trap_table_end - (size_t)&trap_table_start);
	store_sctlr(load_sctlr() | 0 << 13);
	__asm__ __volatile__ ("swi 15");
#endif
#if 0
	int n, m;
	REG_STORE(OMAP35X_INTC_SYSCONFIG, 0x1);
	REG_STORE(OMAP35X_INTC_IDLE, 0x0);

	for (m = 0; m <= 95; ++m) {
		REG_STORE(OMAP35X_INTC_ILR(m), 0x0);
	}

	for (n = 0; n <= 2; ++n) {
		REG_STORE(OMAP35X_INTC_MIR(n), 0x0);
		REG_STORE(OMAP35X_INTC_MIR_CLEAR(n), 0x1);
	}
#endif
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

extern int printf(const char *,...);
void interrupt_handle(void) {
	printf("something happand\n");
}

