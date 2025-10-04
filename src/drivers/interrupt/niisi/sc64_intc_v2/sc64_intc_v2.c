/**
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 17.05.24
 */

#include <assert.h>
#include <endian.h>
#include <stddef.h>
#include <stdint.h>

#include <asm/mipsregs.h>
#include <drivers/common/memory.h>
#include <drivers/irqctrl.h>
#include <framework/mod/options.h>
#include <hal/reg.h>
#include <kernel/irq.h>
#include <util/field.h>
#include <util/macro.h>

#define BASE_ADDR        OPTION_GET(NUMBER, base_addr)
#define HW_INTERRUPT_PIN OPTION_GET(NUMBER, hw_interrupt_pin)

#define INTC_MASK        (BASE_ADDR + 0x000)
#define INTC_VECTOR      (BASE_ADDR + 0x008)
#define INTC_PRIOR(n)    (BASE_ADDR + 0x010 + n)
#define INTC_MAP(n)      (BASE_ADDR + 0x050 + n)
#define INTC_TEST_SET    (BASE_ADDR + 0x080)
#define INTC_TEST_CLR    (BASE_ADDR + 0x088)
#define INTC_MASK_SET    (BASE_ADDR + 0x090)
#define INTC_MASK_CLR    (BASE_ADDR + 0x098)
#define INTC_CORE_NUM    (BASE_ADDR + 0x0a0)
#define INTC_CODE(n)     (BASE_ADDR + 0x100 + n * 4)

#define INTC_MAP_INTn0   0b000001
#define INTC_MAP_INTn1   0b000010
#define INTC_MAP_INTn2   0b000100
#define INTC_MAP_INTn3   0b001000
#define INTC_MAP_INTn4   0b010000
#define INTC_MAP_INTn5   0b100000

#define INTC_VECTOR_INTn0
#define INTC_VECTOR_INTn0_MASK  0x3fUL
#define INTC_VECTOR_INTn0_SHIFT 0

#define INTC_VECTOR_INTn1
#define INTC_VECTOR_INTn1_MASK  0x3fUL
#define INTC_VECTOR_INTn1_SHIFT 8

#define INTC_VECTOR_INTn2
#define INTC_VECTOR_INTn2_MASK  0x3fUL
#define INTC_VECTOR_INTn2_SHIFT 16

#define INTC_VECTOR_INTn3
#define INTC_VECTOR_INTn3_MASK  0x3fUL
#define INTC_VECTOR_INTn3_SHIFT 24

#define INTC_VECTOR_INTn4
#define INTC_VECTOR_INTn4_MASK  0x3fUL
#define INTC_VECTOR_INTn4_SHIFT 32

#define INTC_VECTOR_INTn5
#define INTC_VECTOR_INTn5_MASK  0x3fUL
#define INTC_VECTOR_INTn5_SHIFT 40

#define INTC_VECTOR_INTnX
#define INTC_VECTOR_INTnX_MASK \
	MACRO_FOREACH(MACRO_CONCAT, INTC_VECTOR_INTn, HW_INTERRUPT_PIN, _MASK)
#define INTC_VECTOR_INTnX_SHIFT \
	MACRO_FOREACH(MACRO_CONCAT, INTC_VECTOR_INTn, HW_INTERRUPT_PIN, _SHIFT)

#define INTC_MAP_INTnX MACRO_CONCAT(INTC_MAP_INTn, HW_INTERRUPT_PIN)

static int sc64_intc_v2_init(void) {
	uint32_t c0;
	int i;

	REG64_STORE(INTC_MASK_SET, 0xffffffffffff);

	for (i = 0; i < IRQCTRL_IRQS_TOTAL; i++) {
		REG8_STORE(INTC_MAP(i), INTC_MAP_INTnX);
	}

	c0 = mips_read_c0_status();
	c0 |= 1U << (HW_INTERRUPT_PIN + ST0_SOFTIRQ_NUM + ST0_IRQ_MASK_OFFSET);
	mips_write_c0_status(c0);

	return 0;
}

void irqctrl_enable(unsigned int irq) {
	uint32_t c0;

	assert(irq_nr_valid(irq));

	if (irq < 8) {
		c0 = mips_read_c0_status();
		c0 |= 1U << (irq + ST0_IRQ_MASK_OFFSET);
		mips_write_c0_status(c0);
	}
	else {
		REG64_STORE(INTC_MASK_CLR, htole64(1ULL << irq));
	}
}

void irqctrl_disable(unsigned int irq) {
	uint32_t c0;

	assert(irq_nr_valid(irq));

	if (irq < 8) {
		c0 = mips_read_c0_status();
		c0 &= ~(1U << (irq + ST0_IRQ_MASK_OFFSET));
		mips_write_c0_status(c0);
	}
	else {
		REG64_STORE(INTC_MASK_SET, htole64(1ULL << irq));
	}
}

void irqctrl_force(unsigned int irq) {
	assert(irq_nr_valid(irq));

	REG64_STORE(INTC_TEST_SET, htole64(1ULL << irq));
}

int irqctrl_pending(unsigned int irq) {
	return 0;
}

void irqctrl_eoi(unsigned int irq) {
}

int irqctrl_get_intid(void) {
	unsigned int irq;
	uint32_t pending;

	pending = (mips_read_c0_cause() & CAUSE_IM) >> ST0_IRQ_MASK_OFFSET;

	for (irq = 0; irq < 8; irq++) {
		if (pending & (1U << irq)) {
			break;
		}
	}

	if (irq == 8) {
		return -1;
	}

	if (irq == HW_INTERRUPT_PIN + ST0_SOFTIRQ_NUM) {
		irq = FIELD_GET(le64toh(REG64_LOAD(INTC_VECTOR)), INTC_VECTOR_INTnX);
		if (irq == 0) {
			return -1;
		}
	}

	return irq;
}

IRQCTRL_DEF(sc64_intc_v2, sc64_intc_v2_init);

// PERIPH_MEMORY_DEFINE(sc64_intc_v2_regs, BASE_ADDR, 0x200);
