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

#define CLIC_CFG_NLBITS(n)  (n << 1) /* Interrupt priority bits */
#define CLIC_INTCTL_PRIO(n) (n << 4) /* Interrupt priorty */

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
#if CLIC_VECTORED_MODE
	extern void (*riscv_trap_handler)(void);
	static void *clic_vector_table[] __attribute__((aligned(512))) = {
	    [0 ... IRQCTRL_IRQS_TOTAL] = &riscv_trap_handler};

	csr_set(CSR_TVEC, CSR_TVEC_MODE_VECTORED);
	csr_write(CSR_TVT, clic_vector_table);
#endif

	/* Enable 4 interrupt levels (2 bits) */
	REG8_STORE(CLIC_CFG, CLIC_CFG_NLBITS(2U));

	return 0;
}

void irqctrl_enable(unsigned int irq) {
	REG8_STORE(CLIC_INTATTR(irq), CLIC_INTATTR_INIT);
	REG8_STORE(CLIC_INTCTL(irq), CLIC_INTCTL_PRIO(1));
	REG8_STORE(CLIC_INTIE(irq), 1);
}

void irqctrl_disable(unsigned int irq) {
	REG8_STORE(CLIC_INTIE(irq), 0);
}

void irqctrl_eoi(unsigned int irq) {
	REG8_STORE(CLIC_INTIP(irq), 0);
}

int irqctrl_get_intid(void) {
	int irq;
	int i;

	irq = -1;

	for (i = 0; i < IRQCTRL_IRQS_TOTAL; i++) {
		if (REG8_LOAD(CLIC_INTIP(i))) {
			irq = i;
		}
	}

	return irq;
}

int irqctrl_set_level(unsigned int irq, int level) {
	if (level == 1) {
		return irq;
	}

	return -1;
}

IRQCTRL_DEF(riscv_clic, clic_init);
