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
#include <embox/unit.h>

#define NVIC_BASE 0xe000e100
#define NVIC_ENABLE_BASE NVIC_BASE
#define NVIC_CLEAR_BASE (NVIC_BASE + 0x80)
#define NVIC_SET_PEND_BASE (NVIC_BASE + 0x100)
#define NVIC_CLR_PEND_BASE (NVIC_BASE + 0x180)
#define NVIC_ACTIVE_BASE (NVIC_BASE + 0x200)
#define NVIC_PRIOR_BASE (NVIC_BASE + 0x300)

#define SCB_BASE 0xe000ed00
#define SCB_ICSR  (SCB_BASE + 0x04)
#define SCB_VTOR  (SCB_BASE + 0x08)
#define SCB_SHPR1 (SCB_BASE + 0x18)
#define SCB_SHPR3 (SCB_BASE + 0x20)

#define EXCEPTION_TABLE_SZ OPTION_GET(NUMBER,irq_table_size)

// Table 2.17. Exception return behavior in Cortex-M4 doc
#define interrupted_from_fpu_mode(lr) ((lr & 0xF0) == 0xE0)

/**
 * ENABLE, CLEAR, SET_PEND, CLR_PEND, ACTIVE is a base of bit arrays
 * to calculate bit offset in array: calculate 32-bit word offset
 *     nr / 32 * sizeof(int) == nr / 8
 * and calculate bit offset in word
 *     nr / 32
 */

#ifndef STATIC_IRQ_EXTENTION

EMBOX_UNIT_INIT(nvic_init);

static uint32_t exception_table[EXCEPTION_TABLE_SZ] __attribute__ ((aligned (128 * sizeof(int))));

extern void *trap_table_start;
extern void *trap_table_end;

struct cpu_saved_ctx {
	uint32_t r[5];
	uint32_t lr;
	uint32_t pc;
	uint32_t psr;
};

struct irq_saved_state {
	struct cpu_saved_ctx ctx;
	uint32_t sp;
	uint32_t lr;
};

extern void __irq_trampoline(struct irq_saved_state *state, struct context *regs);
extern void __pending_handle(void);
extern void __pendsv_handle(void);
extern void interrupt_handle_enter(void);

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
void interrupt_handle(struct context *regs) {
	uint32_t source;
	struct irq_saved_state state;
	struct cpu_saved_ctx *ctx;

	source = REG_LOAD(SCB_ICSR) & 0x1ff;

	assert(!critical_inside(CRITICAL_IRQ_LOCK));

	critical_enter(CRITICAL_IRQ_HANDLER);

	irq_dispatch(source);

	critical_leave(CRITICAL_IRQ_HANDLER);

	state.sp = regs->sp;
	state.lr = regs->lr;
	assert(!interrupted_from_fpu_mode(state.lr));
	ctx = (struct cpu_saved_ctx*) state.sp;
	memcpy(&state.ctx, ctx, sizeof *ctx);

	/* It does not matter what value of psr is, just set up sime correct value.
	 * This value is only used to go further, after return from interrupt_handle.
	 * 0x01000000 is a default value of psr and (ctx->psr & 0xFF) is irq number if any. */
	ctx->psr = 0x01000000 | (ctx->psr & 0xFF);
	ctx->r[0] = (uint32_t) &state; // we want pass the state to __pending_handle()
	ctx->r[1] = (uint32_t) regs; // we want pass the registers to __pending_handle()
	ctx->lr = (uint32_t) __pending_handle;
	ctx->pc = ctx->lr;

	/* Now return from interrupt context into __pending_handle */
	__irq_trampoline(&state, regs);
}

void nvic_set_pendsv(void) {
	REG_STORE(SCB_ICSR, 1 << 28);
}

static int nvic_init(void) {
	ipl_t ipl;
	int i;
	void *ptr;

	for (i = 0; i < EXCEPTION_TABLE_SZ; i++) {
		exception_table[i] = ((int) interrupt_handle_enter) | 1;
	}
	assert(EXCEPTION_TABLE_SZ >= 14);
	exception_table[14] = ((int) __pendsv_handle) | 1;

	/* load head from bootstrap table */
	for (ptr = &trap_table_start, i = 0; ptr != &trap_table_end; ptr += 4, i++) {
		exception_table[i] = * (int32_t *) ptr;
	}

	ipl = ipl_save();

	REG_STORE(SCB_VTOR, 1 << 29 /* indicate, table in SRAM */ |
			(int) exception_table);

	ipl_restore(ipl);

	return 0;
}

#endif

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
