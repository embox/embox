/**
 * @file
 *
 * @author Efim Perevalov
 * @date 8.12.2025
 */
#include <asm/csr.h>
#include <drivers/irqctrl.h>
#include <framework/mod/options.h>
#include <hal/reg.h>

#define BASE_ADDR OPTION_GET	(NUMBER, base_addr)
#define MSTATUS_MIE         	0x00000008

#define INTERRUPT_CORE0_CPU_INT_ENABLE		(BASE_ADDR + 0x0104)
#define INTERRUPT_CORE0_CPU_INT_THRESH_REG	(BASE_ADDR + 0x0194)

#define mcause	0x342
#define mstatus	0x300

#define INTERRUPT_CORE0_INTR_MAP(irq) (BASE_ADDR + irq * 4)
#define CPU_LINE 4

void map_irq_to_cpu(unsigned int irq, unsigned int cpu_line) {
    REG32_STORE(INTERRUPT_CORE0_INTR_MAP(irq), cpu_line);
}

void irqctrl_enable(unsigned int irq) {
	map_irq_to_cpu(irq, CPU_LINE);
	REG32_ORIN(INTERRUPT_CORE0_CPU_INT_ENABLE, 1 << CPU_LINE);
}

void irqctrl_disable(unsigned int irq) {
	REG32_CLEAR(INTERRUPT_CORE0_CPU_INT_ENABLE, 1 << CPU_LINE);
}

void irqctrl_eoi(unsigned int irq) {
}

int irqctrl_get_intid(void) {
	return csr_read(mcause) & 0x3FF;
}

static inline void rv_utils_restore_intlevel_regval(uint32_t restoreval)
{
    REG32_STORE(INTERRUPT_CORE0_CPU_INT_THRESH_REG, restoreval);
}

int irqctrl_set_level(unsigned int irq, int level) {
	uint32_t old_mstatus = csr_read(mstatus);
	csr_clear(mstatus, MSTATUS_MIE);
    uint32_t old_thresh = REG32_LOAD(INTERRUPT_CORE0_CPU_INT_THRESH_REG);
    rv_utils_restore_intlevel_regval(level);
    csr_set(mstatus, old_mstatus & MSTATUS_MIE);

    return old_thresh;
}

static int esp32c3_intc_init(void) {
	extern void (*vector_table)(void);
	// static void *_vector_table[] __attribute__((aligned(256))) = {
	//     [0 ... IRQCTRL_IRQS_TOTAL] = &vector_table};

	csr_write(CSR_TVEC, &vector_table);
	csr_set(CSR_TVEC, CSR_TVEC_MODE_VECTORED);

	REG32_STORE(INTERRUPT_CORE0_CPU_INT_ENABLE, 0);
	csr_set(mstatus, MSTATUS_MIE);

	return 0;
}

IRQCTRL_DEF(esp32c3_intc, esp32c3_intc_init);
