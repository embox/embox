/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    02.07.2012
 */

#include <assert.h>
#include <stdint.h>
#include <string.h>

#include <kernel/critical.h>
#include <hal/reg.h>
#include <hal/ipl.h>
#include <hal/context.h>
#include <drivers/irqctrl.h>

#include <kernel/irq.h>
#include <util/log.h>
#include <embox/unit.h>

#define NVIC_BASE 0xe000e100
#define NVIC_ENABLE_BASE NVIC_BASE
#define NVIC_CLEAR_BASE (NVIC_BASE + 0x80)
#define NVIC_SET_PEND_BASE (NVIC_BASE + 0x100)
#define NVIC_CLR_PEND_BASE (NVIC_BASE + 0x180)
#define NVIC_ACTIVE_BASE (NVIC_BASE + 0x200)
#define NVIC_PRIORITY_BASE (NVIC_BASE + 0x300)

#define SCB_BASE 0xe000ed00
#define SCB_ICSR  (SCB_BASE + 0x04)
#define SCB_VTOR  (SCB_BASE + 0x08)
#define SCB_SHPR1 (SCB_BASE + 0x18)
#define SCB_SHPR2 (SCB_BASE + 0x1C)
# define SCB_PRIO_SVCALL   24
#define SCB_SHPR3 (SCB_BASE + 0x20)
# define SCB_PRIO_PENDSV   16
# define SCB_PRIO_SYSTICK  24
#define SCB_SHCRS (SCB_BASE + 0x24)
# define SCB_SHCRS_EN_MEM_FAULT   16
# define SCB_SHCRS_EN_BUS_FAULT   17
# define SCB_SHCRS_EN_USAGE_FAULT 18

#define EXCEPTION_TABLE_SZ OPTION_GET(NUMBER,irq_table_size)

// Table 2.17. Exception return behavior in Cortex-M4 doc
#define interrupted_from_fpu_mode(lr) ((lr & 0xF0) == 0xE0)

#define FPU_CTX_SIZE   18

/**
 * ENABLE, CLEAR, SET_PEND, CLR_PEND, ACTIVE is a base of bit arrays
 * to calculate bit offset in array: calculate 32-bit word offset
 *     nr / 32 * sizeof(int) == nr / 8
 * and calculate bit offset in word
 *     nr / 32
 */

EMBOX_UNIT_INIT(nvic_init);

static void nvic_setup_priorities(void);
extern void __pendsv_handle(void);

#ifndef STATIC_IRQ_EXTENTION

static uint32_t exception_table[EXCEPTION_TABLE_SZ] __attribute__ ((aligned (128 * sizeof(int))));

extern void *trap_table_start;
extern void *trap_table_end;

extern void interrupt_handle_enter(void);

static void hnd_stub(void) {
	/* It's just a stub. DO NOTHING */
}

void nvic_table_fill_stubs(void) {
	int i;

	for (i = 0; i < EXCEPTION_TABLE_SZ; i++) {
		exception_table[i] = ((int) hnd_stub) | 1;
	}

	REG_STORE(SCB_VTOR, 1 << 29 /* indicate, table in SRAM */ |
			(int) exception_table);
}

static int nvic_init(void) {
	ipl_t ipl;
	int i;
	void *ptr;

	ipl = ipl_save();

	for (i = 0; i < EXCEPTION_TABLE_SZ; i++) {
		irqctrl_disable(i);
		exception_table[i] = ((int) interrupt_handle_enter) | 1;
	}

	/* load head from bootstrap table */
	for (ptr = &trap_table_start, i = 0; ptr != &trap_table_end; ptr += 4, i++) {
		exception_table[i] = * (int32_t *) ptr;
	}

	assert(EXCEPTION_TABLE_SZ >= 14);
	exception_table[14] = ((int) __pendsv_handle) | 1;

	REG_STORE(SCB_VTOR, 1 << 29 /* indicate, table in SRAM */ |
			(int) exception_table);

	ipl_restore(ipl);

	nvic_setup_priorities();

	return 0;
}

#else

void nvic_table_fill_stubs(void) {

}

static int nvic_init(void) {
	nvic_setup_priorities();
	return 0;
}

ARM_M_IRQ_HANDLER_DEF(14, __pendsv_handle);

#endif

struct irq_enter_ctx {
	uint32_t r[13];
	uint32_t lr;
	uint32_t sp;
	uint32_t control;
};

struct irq_saved_state {
	struct exc_saved_base_ctx ctx;
	uint32_t misc[20];
} __attribute__((packed));

extern void __irq_trampoline(uint32_t sp, uint32_t lr);
extern void __pending_handle(void);

/*
 * Usual interrupt handling:
 *     execution flow
 *         |
 *         |
 *         v
 *         |----irq enter----> interrupt_handle (irq context)
 *                                |
 *         <----irq exit----------|
 *         |
 *         |
 *         v
 *
 * This method:
 *     execution flow
 *         |
 *         |
 *         v
 *         |---irq enter------> interrupt_handle (irq context)
 *                        |
 *                        |
 *               __irq_trampoline---- irq exit--------> __pending_handle (non-irq context)
 *                                                   |
 *                                                   |----irq enter------> __pendsv_handle (irq context)
 *                                                                                 |
 *          <--------------------------irq exit------------------------------------|
 *          |
 *          |
 *          v
 */
static void fill_irq_saved_ctx(struct irq_saved_state *state,
               uint32_t *opaque, struct irq_enter_ctx *regs) {
	struct exc_saved_base_ctx *ctx;
	size_t offset;

	ctx = (struct exc_saved_base_ctx *) opaque;

	/* Save LR and SP below the context. */
	offset = interrupted_from_fpu_mode(regs->lr) ? FPU_CTX_SIZE : 0;
	state->misc[offset] = regs->lr;
	state->misc[offset + 1] = regs->sp;

	/* It does not matter what value of psr is, just set up sime correct value.
	 * This value is only used to go further, after return from interrupt_handle.
	 * 0x01000000 is a default value of psr and (ctx->psr & 0xFF) is irq number if any. */
	state->ctx.psr = 0x01000000 | (ctx->psr & 0xFF);
	state->ctx.r[0] = (uint32_t) regs->lr; // pass LR to __pending_handle()
	state->ctx.r[1] = (uint32_t) regs; // pass the registers to __pending_handle()
	state->ctx.lr = (uint32_t) __pending_handle;
	state->ctx.pc = state->ctx.lr;
}

void interrupt_handle(struct irq_enter_ctx *regs,
		struct irq_saved_state *state) {
#ifndef STATIC_IRQ_EXTENTION
	uint32_t source;

	source = REG_LOAD(SCB_ICSR) & 0x1ff;

	assert(!critical_inside(CRITICAL_IRQ_LOCK));

	critical_enter(CRITICAL_IRQ_HANDLER);
	irq_dispatch(source);
	critical_leave(CRITICAL_IRQ_HANDLER);
#endif

	/* Now return from irq */

	fill_irq_saved_ctx(state, (uint32_t *) regs->sp, regs);

	/* Now return from interrupt context into __pending_handle.
	 * Pass &state as a top of 'interrupted' stack to use state.ctx
	 * for returning into __pending_handle. */
	__irq_trampoline((uint32_t) state, regs->lr);
}

void nvic_set_pendsv(void) {
	REG_STORE(SCB_ICSR, 1 << 28);
}

static void nvic_setup_priorities(void) {
	int i;

	/* Enable MemManage, BusFault and UsageFault */
	REG_STORE(SCB_SHCRS,
		(1 << SCB_SHCRS_EN_MEM_FAULT) |
		(1 << SCB_SHCRS_EN_BUS_FAULT) |
		(1 << SCB_SHCRS_EN_USAGE_FAULT));

	REG8_STORE(SCB_SHPR1 + 0, 0);
	REG8_STORE(SCB_SHPR1 + 1, 0);
	REG8_STORE(SCB_SHPR1 + 2, 0);

	/* Set priorities of PendSV to maximum. It can be used it the following
	* way. When we use interrupts priorities, we can generate PendSV from
	* any other irq handler, because it's of the maximal priority. */
	REG8_STORE(SCB_SHPR3 + 2, NVIC_MAX_PRIO);

	/* Set to all other interrupts priority to minimal  */
	REG8_STORE(SCB_SHPR3 + 3, NVIC_MIN_PRIO);
	for (i = 0; i < 240; i++) {
		REG8_STORE(NVIC_PRIORITY_BASE + i, NVIC_MIN_PRIO);
	}
}

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
		log_error("irq prio > 15\n");
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
