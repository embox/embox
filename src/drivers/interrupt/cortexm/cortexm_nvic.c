/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    02.07.2012
 */

#include <assert.h>
#include <string.h>
#include <hal/reg.h>
#include <hal/ipl.h>
#include <asm/arm_m_regs.h>
#include <arm/exception.h>
#include <drivers/irqctrl.h>
#include <kernel/irq.h>
#include <kernel/critical.h>
#include <embox/unit.h>

#define NVIC_BASE          0xe000e100
#define NVIC_ENABLE_BASE   NVIC_BASE
#define NVIC_CLEAR_BASE    (NVIC_BASE + 0x80)
#define NVIC_SET_PEND_BASE (NVIC_BASE + 0x100)
#define NVIC_CLR_PEND_BASE (NVIC_BASE + 0x180)
#define NVIC_PRIORITY_BASE (NVIC_BASE + 0x300)

#define NVIC_PRIO_SHIFT    4

#define EXCEPTION_TABLE_SZ OPTION_GET(NUMBER,irq_table_size)

EMBOX_UNIT_INIT(nvic_init);

#ifndef STATIC_IRQ_EXTENTION
static uint32_t exception_table[EXCEPTION_TABLE_SZ] __attribute__ ((aligned (128 * sizeof(int))));

extern void *trap_table_start;
extern void *trap_table_end;

extern void arm_m_irq_entry(void);

static int nvic_init(void) {
	ipl_t ipl;
	int i;
	int trap_table_len = ((int) &trap_table_end - (int) &trap_table_start) / 4;

	ipl = ipl_save();

	/* Copy exception table. */
	memcpy(&exception_table[0], &trap_table_start, trap_table_len * 4);

	for (i = trap_table_len; i < EXCEPTION_TABLE_SZ; i++) {
		exception_table[i] = ((int) arm_m_irq_entry) | 1;
	}

	REG_STORE(SCB_VTOR, SCB_VTOR_IN_RAM | (int) exception_table);

	ipl_restore(ipl);

	return 0;
}

void nvic_irq_handle(void) {
	uint32_t source;

	source = (REG_LOAD(SCB_ICSR) & 0x1ff) - 16;

	critical_enter(CRITICAL_IRQ_HANDLER);
	irq_dispatch(source);
	critical_leave(CRITICAL_IRQ_HANDLER);
}
#else /* STATIC_IRQ_EXTENTION */
static int nvic_init(void) {
	return 0;
}
#endif /* STATIC_IRQ_EXTENTION */

void irqctrl_enable(unsigned int nr) {
	REG_STORE(NVIC_ENABLE_BASE + 4 * (nr / 32), 1 << (nr % 32));
}

void irqctrl_disable(unsigned int nr) {
	REG_STORE(NVIC_CLEAR_BASE + 4 * (nr / 32), 1 << (nr % 32));
}

void irqctrl_clear(unsigned int nr) {
	REG_STORE(NVIC_CLR_PEND_BASE + 4 * (nr / 32), 1 << (nr % 32));
}

void irqctrl_force(unsigned int nr) {
	REG_STORE(NVIC_SET_PEND_BASE + 4 * (nr / 32), 1 << (nr % 32));
}

void irqctrl_set_prio(unsigned int nr, unsigned int prio) {
	if (prio > 15) {
		return;
	}
	/* In NVIC the lower priopity means higher IRQ prioriry. */
	prio = 15 - prio;

	REG8_STORE(NVIC_PRIORITY_BASE + nr,
		((prio << NVIC_PRIO_SHIFT) & 0xff));
}

unsigned int irqctrl_get_prio(unsigned int nr) {
	/* In NVIC the lower priopity means higher IRQ prioriry. */
	return 15 - (REG8_LOAD(NVIC_PRIORITY_BASE + nr) >> NVIC_PRIO_SHIFT);
}
