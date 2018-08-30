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

#define BASE_CTX_SIZE  (8 * 4)
#define FPU_CTX_SIZE   (BASE_CTX_SIZE + 18 * 4)

#ifndef IRQ_PRIO_SHIFT
#define IRQ_PRIO_SHIFT 4
#define IRQ_MIN_NONFAULT_PRIO (1 << IRQ_PRIO_SHIFT)
#endif

/**
 * ENABLE, CLEAR, SET_PEND, CLR_PEND, ACTIVE is a base of bit arrays
 * to calculate bit offset in array: calculate 32-bit word offset
 *     nr / 32 * sizeof(int) == nr / 8
 * and calculate bit offset in word
 *     nr / 32
 */

EMBOX_UNIT_INIT(nvic_init);

static void nvic_setup_priorities(void);

#ifndef STATIC_IRQ_EXTENTION

static uint32_t exception_table[EXCEPTION_TABLE_SZ] __attribute__ ((aligned (128 * sizeof(int))));

extern void *trap_table_start;
extern void *trap_table_end;

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
static void fill_irq_saved_ctx(struct irq_saved_state *state,
               uint32_t *opaque, struct irq_enter_ctx *regs) {
	struct exc_saved_base_ctx *ctx;

	ctx = (struct exc_saved_base_ctx *) opaque;

	if (!interrupted_from_fpu_mode(regs->lr)) {
		memcpy(&state->ctx, ctx, BASE_CTX_SIZE);
		state->misc[0] = regs->lr;
		state->misc[1] = regs->sp;
	} else {
		memcpy(&state->ctx, ctx, FPU_CTX_SIZE);
		state->misc[18] = regs->lr;
		state->misc[19] = regs->sp;
	}

	/* It does not matter what value of psr is, just set up sime correct value.
	 * This value is only used to go further, after return from interrupt_handle.
	 * 0x01000000 is a default value of psr and (ctx->psr & 0xFF) is irq number if any. */
	state->ctx.psr = 0x01000000 | (ctx->psr & 0xFF);
	state->ctx.r[0] = (uint32_t) state; // pass the state to __pending_handle()
	state->ctx.r[1] = (uint32_t) regs; // pass the registers to __pending_handle()
	state->ctx.lr = (uint32_t) __pending_handle;
	state->ctx.pc = state->ctx.lr;
}


void interrupt_handle(struct irq_enter_ctx *regs,
		struct irq_saved_state *state) {
	uint32_t source;

	source = REG_LOAD(SCB_ICSR) & 0x1ff;

	assert(!critical_inside(CRITICAL_IRQ_LOCK));

	irqctrl_disable(source);
	critical_enter(CRITICAL_IRQ_HANDLER);
	{
		ipl_enable();

		irq_dispatch(source);

		ipl_disable();
	}
	irqctrl_enable(source);
	critical_leave(CRITICAL_IRQ_HANDLER);

	fill_irq_saved_ctx(state, (uint32_t *) regs->sp, regs);

	/* Now return from interrupt context into __pending_handle.
	 * Pass &state as a top of 'interrupted' stack to use state.ctx
	 * for returning into __pending_handle. */
	__irq_trampoline((uint32_t) state, regs->lr);
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

	/* load head from bootstrap table */
	for (ptr = &trap_table_start, i = 0; ptr != &trap_table_end; ptr += 4, i++) {
		exception_table[i] = * (int32_t *) ptr;
	}

	assert(EXCEPTION_TABLE_SZ >= 14);
	exception_table[14] = ((int) __pendsv_handle) | 1;

	ipl = ipl_save();

	REG_STORE(SCB_VTOR, 1 << 29 /* indicate, table in SRAM */ |
			(int) exception_table);

	ipl_restore(ipl);

	nvic_setup_priorities();

	return 0;
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

#else

/* These functions are used in src/drivers/interrupt/cortexm_irq_handle.S */

void interrupt_handle(struct context *regs) {
}

void nvic_set_pendsv(void) {
}

void nvic_table_fill_stubs(void) {

}

static int nvic_init(void) {
	nvic_setup_priorities();
	return 0;
}

#endif

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

	/* Set priorities of Systick and PendSV to 1 */
	REG8_STORE(SCB_SHPR3 + 2, IRQ_MIN_NONFAULT_PRIO);
	REG8_STORE(SCB_SHPR3 + 3, IRQ_MIN_NONFAULT_PRIO);

	/* Set to all other interrupts priority equal to 1 */
	for (i = 0; i < 240; i++) {
		REG8_STORE(NVIC_PRIORITY_BASE + i, IRQ_MIN_NONFAULT_PRIO);
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
