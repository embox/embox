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

#define EXCEPTION_TABLE_SZ OPTION_GET(NUMBER,irq_table_size)

EMBOX_UNIT_INIT(nvic_init);

#ifndef STATIC_IRQ_EXTENTION
static uint32_t exception_table[EXCEPTION_TABLE_SZ] __attribute__ ((aligned (128 * sizeof(int))));

extern void *trap_table_start;
extern void *trap_table_end;

extern void arm_m_irq_entry(void);
extern void arm_m_pendsv_entry(void);

static void hnd_stub(void) {
	/* It's just a stub. DO NOTHING */
}

/* TODO Should be removed at all. */
void nvic_table_fill_stubs(void) {
	int i;

	for (i = 0; i < EXCEPTION_TABLE_SZ; i++) {
		exception_table[i] = ((int) hnd_stub) | 1;
	}
	REG_STORE(SCB_VTOR, SCB_VTOR_IN_RAM | (int) exception_table);
}

static int nvic_init(void) {
	ipl_t ipl;
	int i, trap_table_size;

	trap_table_size = ((int) &trap_table_end - (int) &trap_table_start) / 4;
	assert(EXCEPTION_TABLE_SZ >= trap_table_size);

	ipl = ipl_save();

	for (i = trap_table_size; i < EXCEPTION_TABLE_SZ; i++) {
		exception_table[i] = ((int) arm_m_irq_entry) | 1;
	}
	/* load head from bootstrap table */
	memcpy(&exception_table[0], &trap_table_start, trap_table_size * 4);
	exception_table[14] = ((int) arm_m_pendsv_entry) | 1;

	REG_STORE(SCB_VTOR, SCB_VTOR_IN_RAM | (int) exception_table);

	ipl_restore(ipl);

	return 0;
}

void nvic_irq_handle(void) {
	uint32_t source;

	source = REG_LOAD(SCB_ICSR) & 0x1ff;

	critical_enter(CRITICAL_IRQ_HANDLER);
	irq_dispatch(source);
	critical_leave(CRITICAL_IRQ_HANDLER);
}
#else /* STATIC_IRQ_EXTENTION */
void nvic_table_fill_stubs(void) {

}

static int nvic_init(void) {
	return 0;
}
#endif /* STATIC_IRQ_EXTENTION */

void irqctrl_enable(unsigned int interrupt_nr) {
	int nr = (int) interrupt_nr - 16;
	if (nr >= 0) {
		REG_STORE(NVIC_ENABLE_BASE + 4 * (nr / 32), 1 << (nr % 32));
	}
}

void irqctrl_disable(unsigned int interrupt_nr) {
	int nr = (int) interrupt_nr - 16;
	if (nr >= 0) {
		REG_STORE(NVIC_CLEAR_BASE + 4 * (nr / 32), 1 << (nr % 32));
	}
}

void irqctrl_clear(unsigned int interrupt_nr) {
	int nr = (int) interrupt_nr - 16;
	if (nr >= 0) {
		REG_STORE(NVIC_CLR_PEND_BASE + 4 * (nr / 32), 1 << (nr % 32));
	}
}

void irqctrl_force(unsigned int interrupt_nr) {
	int nr = (int) interrupt_nr - 16;
	if (nr >= 0) {
		REG_STORE(NVIC_SET_PEND_BASE + 4 * (nr / 32), 1 << (nr % 32));
	}
}

void irqctrl_set_prio(unsigned int interrupt_nr, unsigned int prio) {
	int nr = (int) interrupt_nr - 16;

	if (prio > 15) {
		return;
	}
	/* In NVIC the lower priopity means higher IRQ prioriry. */
	prio = 15 - prio;

	if (nr >= 0) {
		REG8_STORE(NVIC_PRIORITY_BASE + nr,
			((prio << NVIC_PRIO_SHIFT) & 0xff));
	}
}

unsigned int irqctrl_get_prio(unsigned int interrupt_nr) {
	int nr = (int) interrupt_nr - 16;
	if (nr >= 0) {
		/* In NVIC the lower priopity means higher IRQ prioriry. */
		return 15 - REG8_LOAD(NVIC_PRIORITY_BASE + nr);
	}
	return 0;
}
