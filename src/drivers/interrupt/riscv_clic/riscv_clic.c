/**
 * @file
 * @brief RISC-V Core Local Interrupt Controller (CLIC)
 *
 * @author Aleksey Zhmulin
 * @date 19.08.25
 */

#include <stdint.h>

#include <asm/csr.h>
#include <drivers/irqctrl.h>
#include <framework/mod/options.h>
#include <hal/reg.h>

#define CLIC_BASE_ADDR     OPTION_GET(NUMBER, base_addr)
#define CLIC_VECTORED_MODE OPTION_GET(BOOLEAN, vectored_mode)

/* clang-format off */
#define CLIC_CFG        (CLIC_BASE_ADDR + 0)              /* Configuration register */
#define CLIC_INFO       (CLIC_BASE_ADDR + 4)              /* Information register */
#define CLIC_INTIP(n)   (CLIC_BASE_ADDR + 0x1000 + 4 * n) /* Interrupt pending registers */
#define CLIC_INTIE(n)   (CLIC_BASE_ADDR + 0x1001 + 4 * n) /* Interrupt enable registers */
#define CLIC_INTATTR(n) (CLIC_BASE_ADDR + 0x1002 + 4 * n) /* Interrupt attribute registers */
#define CLIC_INTCTL(n)  (CLIC_BASE_ADDR + 0x1003 + 4 * n) /* Interrupt control registers */
/* clang-format on */

#if defined(__EMBOX_PLATFORM__esp32c5__)
#define CLIC_CFG_NLBITS(n)     (n) /* MNLBITS occupies bits 3:0. */
/* Two level bits (7:6), one priority bit (5): level 1, priority 0. */
#define CLIC_INTCTL_INIT       (1U << 6)
#define CLIC_VECTOR_ALIGNMENT  64
#define CLIC_CAUSE_CODE_MASK   0x3fUL
#define CLIC_MINTTHRESH        0x347
#else
#define CLIC_CFG_NLBITS(n)     ((n) << 1)
#define CLIC_INTCTL_INIT       (1U << 4)
#define CLIC_VECTOR_ALIGNMENT  512
#endif

#define CLIC_INTATTR_SHV       (1U << 0) /* Vectored mode */
#define CLIC_INTATTR_TRIG_LVL1 (0U << 1) /* High-level trigger */
#define CLIC_INTATTR_TRIG_POS  (1U << 1) /* Positive edge trigger */
#define CLIC_INTATTR_TRIG_LVL0 (2U << 1) /* Low-Level trigger */
#define CLIC_INTATTR_TRIG_NEG  (3U << 1) /* Negative edge trigger */
#define CLIC_INTATTR_MODE_U    (0U << 6) /* User mode */
#define CLIC_INTATTR_MODE_S    (1U << 6) /* Supervisor mode */
#define CLIC_INTATTR_MODE_M    (3U << 6) /* Machine mode */

#if CLIC_VECTORED_MODE
#define CLIC_INTATTR_INIT \
	(CLIC_INTATTR_MODE_M | CLIC_INTATTR_TRIG_LVL1 | CLIC_INTATTR_SHV)
#else
#define CLIC_INTATTR_INIT (CLIC_INTATTR_MODE_M | CLIC_INTATTR_TRIG_LVL1)
#endif

static int clic_init(void) {
	extern void riscv_trap_handler(void);

#if defined(__EMBOX_PLATFORM__esp32c5__)
	unsigned int i;

	/* Do not inherit interrupt enables or a threshold from the ROM. */
	for (i = 0; i < IRQCTRL_IRQS_TOTAL; i++) {
		REG8_STORE(CLIC_INTIE(i), 0);
	}
	csr_write(CLIC_MINTTHRESH, 0);

	/* MODE is hardwired to CLIC (3); SHV selects hardware vectoring. */
	csr_write(CSR_TVEC, (uintptr_t)riscv_trap_handler);
#endif

#if CLIC_VECTORED_MODE
	static void (*clic_vector_table[])(void)
	    __attribute__((aligned(CLIC_VECTOR_ALIGNMENT))) = {
	    [0 ... IRQCTRL_IRQS_TOTAL - 1] = &riscv_trap_handler};

#if !defined(__EMBOX_PLATFORM__esp32c5__)
	csr_set(CSR_TVEC, CSR_TVEC_MODE_VECTORED);
#endif
	csr_write(CSR_TVT, clic_vector_table);
#endif

	/* Enable 4 interrupt levels (2 bits) */
	REG8_STORE(CLIC_CFG, CLIC_CFG_NLBITS(2U));

	return 0;
}

void irqctrl_enable(unsigned int irq) {
	REG8_STORE(CLIC_INTATTR(irq), CLIC_INTATTR_INIT);
	REG8_STORE(CLIC_INTCTL(irq), CLIC_INTCTL_INIT);
	REG8_STORE(CLIC_INTIE(irq), 1);
}

void irqctrl_disable(unsigned int irq) {
	REG8_STORE(CLIC_INTIE(irq), 0);
}

void irqctrl_eoi(unsigned int irq) {
	REG8_STORE(CLIC_INTIP(irq), 0);
}

int irqctrl_get_intid(void) {
#if defined(__EMBOX_PLATFORM__esp32c5__)
	unsigned long cause = csr_read(CSR_CAUSE);
	unsigned int irq = cause & CLIC_CAUSE_CODE_MASK;

	if (!(cause & CSR_CAUSE_INT) || irq >= IRQCTRL_IRQS_TOTAL) {
		return -1;
	}

	return irq;
#else
	int irq;
	int i;

	irq = -1;

	for (i = 0; i < IRQCTRL_IRQS_TOTAL; i++) {
		if (REG8_LOAD(CLIC_INTIP(i))) {
			irq = i;
		}
	}

	return irq;
#endif
}

int irqctrl_set_level(unsigned int irq, int level) {
	if (level == 1) {
		return irq;
	}

	return -1;
}

IRQCTRL_DEF(riscv_clic, clic_init);
