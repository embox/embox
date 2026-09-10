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

#define INTERRUPT_CORE0_CPU_INT_PRI(cpu_line)	(BASE_ADDR + 0x0118 + 0x4*(cpu_line - 1))
#define INTERRUPT_CORE0_INTR_MAP(irq) 			(BASE_ADDR + irq * 4)


#define CPU_LINE_MIN        1
#define CPU_LINE_MAX        31
#define CPU_LINE_COUNT      (CPU_LINE_MAX - CPU_LINE_MIN + 1)

#define INT_MUX_DISABLED_INTNO 6

static int irq_to_line[__IRQCTRL_IRQS_TOTAL];
static int line_to_irq[CPU_LINE_COUNT];

static int find_free_cpu_line(void) {
    for (int i = 0; i < CPU_LINE_COUNT; i++) {
        if (line_to_irq[i] == -1) {
            return CPU_LINE_MIN + i;
        }
    }
    return -1;
}

static void map_irq_to_cpu(unsigned int irq, unsigned int cpu_line) {
    int idx = cpu_line - CPU_LINE_MIN;
    REG32_STORE(INTERRUPT_CORE0_INTR_MAP(irq), cpu_line);
    irq_to_line[irq] = cpu_line;
    line_to_irq[idx] = irq;
}

static void unmap_irq_from_cpu(unsigned int irq) {
    unsigned int cpu_line = irq_to_line[irq];
    int idx = cpu_line - CPU_LINE_MIN;

    irq_to_line[irq] = -1;
    line_to_irq[idx] = -1;

    // REG32_CLEAR(INTERRUPT_CORE0_CPU_INT_ENABLE, 1 << cpu_line);
    REG32_STORE(INTERRUPT_CORE0_INTR_MAP(irq), INT_MUX_DISABLED_INTNO);
}

static void set_priority(unsigned int cpu_line, unsigned int priority) {
    REG32_STORE(INTERRUPT_CORE0_CPU_INT_PRI(cpu_line), priority);
}

void irqctrl_enable(unsigned int irq) {
    if (irq_to_line[irq] < 0) {
        int cpu_line = find_free_cpu_line();
        if (cpu_line < 0) {
            return;
        }
        map_irq_to_cpu(irq, cpu_line);
    }

    unsigned int cpu_line = irq_to_line[irq];
    REG32_ORIN(INTERRUPT_CORE0_CPU_INT_ENABLE, 1 << cpu_line);
    set_priority(cpu_line, 1);
}

void irqctrl_disable(unsigned int irq) {
    unsigned int cpu_line = irq_to_line[irq];
    REG32_CLEAR(INTERRUPT_CORE0_CPU_INT_ENABLE, 1 << cpu_line);
    set_priority(cpu_line, 0);

    unmap_irq_from_cpu(irq);
}

void irqctrl_eoi(unsigned int irq) {
}

int irqctrl_get_intid(void) {
    unsigned int cpu_line = csr_read(CSR_CAUSE) & 0x3FF;
    int idx = cpu_line - CPU_LINE_MIN;

    return line_to_irq[idx];
}

static inline void rv_utils_restore_intlevel_regval(uint32_t restoreval) {
    REG32_STORE(INTERRUPT_CORE0_CPU_INT_THRESH_REG, restoreval);
}

int irqctrl_set_level(unsigned int irq, int level) {
    uint32_t old_mstatus = csr_read(CSR_STATUS);
    csr_clear(CSR_STATUS, MSTATUS_MIE);
    uint32_t old_thresh = REG32_LOAD(INTERRUPT_CORE0_CPU_INT_THRESH_REG);
    rv_utils_restore_intlevel_regval(level);
    csr_set(CSR_STATUS, old_mstatus & MSTATUS_MIE);

    return old_thresh;
}

static int esp32c3_intc_init(void) {
    extern void (*vector_table)(void);

    for (int i = 0; i < __IRQCTRL_IRQS_TOTAL; i++) {
        irq_to_line[i] = -1;
    }
    for (int i = 0; i < CPU_LINE_COUNT; i++) {
        line_to_irq[i] = -1;
    }

    csr_write(CSR_TVEC, &vector_table);
    csr_set(CSR_TVEC, CSR_TVEC_MODE_VECTORED);

    REG32_STORE(INTERRUPT_CORE0_CPU_INT_ENABLE, 0);
    csr_set(CSR_STATUS, MSTATUS_MIE);

    rv_utils_restore_intlevel_regval(1);

    return 0;
}

IRQCTRL_DEF(esp32c3_intc, esp32c3_intc_init);
