/**
 * @file
 *
 * @date 31.01.25
 * @author Anton Bondarev
 */

#include <assert.h>
#include <stdint.h>

#include <asm/interrupts.h>
#include <drivers/irqctrl.h>
#include <asm/csr.h>

#include <framework/mod/options.h>

#define MMODE_BASE_ADDR            OPTION_GET(NUMBER, mmode_base_addr)
#define SMODE_BASE_ADDR            OPTION_GET(NUMBER, smode_base_addr)

#define PLF_CORE_VARIANT_SCR       OPTION_GET(NUMBER, scr_ver)

#define PLF_IPIC_MBASE (MMODE_BASE_ADDR)

#define IPIC_CISV  (PLF_IPIC_MBASE + 0)
#define IPIC_CICSR (PLF_IPIC_MBASE + 1)
#define IPIC_IPR   (PLF_IPIC_MBASE + 2)
#define IPIC_ISVR  (PLF_IPIC_MBASE + 3)
#define IPIC_EOI   (PLF_IPIC_MBASE + 4)
#define IPIC_SOI   (PLF_IPIC_MBASE + 5)
#define IPIC_IDX   (PLF_IPIC_MBASE + 6)
#define IPIC_ICSR  (PLF_IPIC_MBASE + 7)
#define IPIC_IER   (PLF_IPIC_MBASE + 8)
#define IPIC_IMAP  (PLF_IPIC_MBASE + 9)

#define IPIC_IRQ_PENDING       (1 << 0)
#define IPIC_IRQ_ENABLE        (1 << 1)
#define IPIC_IRQ_LEVEL         (0 << 2)
#define IPIC_IRQ_EDGE          (1 << 2)

#define IPIC_IRQ_INV           (1 << 3)
#define IPIC_IRQ_MODE_MASK     (3 << 2)
#define IPIC_IRQ_CLEAR_PENDING IPIC_IRQ_PENDING

#define IPIC_IRQ_IN_SERVICE (1 << 4) // RO
#define IPIC_IRQ_PRIV_MASK  (3 << 8)


#define IPIC_IRQ_PRIV_MMODE (3 << 8)
#define IPIC_IRQ_PRIV_SMODE (1 << 8)
#define IPIC_IRQ_LN_OFFS    (12)

#ifndef PLF_IPIC_STATIC_LINE_MAPPING
#define PLF_IPIC_STATIC_LINE_MAPPING ((PLF_CORE_VARIANT_SCR) == 1)
#endif // PLF_IPIC_STATIC_LINE_MAPPING

#ifndef PLF_IPIC_IRQ_LN_NUM
#if (PLF_CORE_VARIANT_SCR) == 1
#define PLF_IPIC_IRQ_LN_NUM (16)
#else
#define PLF_IPIC_IRQ_LN_NUM (32)
#endif
#endif // PLF_IPIC_IRQ_LN_NUM

#ifndef PLF_IPIC_IRQ_VEC_NUM
#if PLF_IPIC_STATIC_LINE_MAPPING
#define PLF_IPIC_IRQ_VEC_NUM PLF_IPIC_IRQ_LN_NUM
#else // PLF_IPIC_STATIC_LINE_MAPPING
#define PLF_IPIC_IRQ_VEC_NUM (32)
#endif // PLF_IPIC_STATIC_LINE_MAPPING
#endif // PLF_IPIC_IRQ_VEC_NUM

#define IPIC_IRQ_LN_VOID  PLF_IPIC_IRQ_LN_NUM
#define IPIC_IRQ_VEC_VOID PLF_IPIC_IRQ_VEC_NUM


#define MK_IRQ_CFG(line, mode, flags) ((mode) | (flags) | ((line) << IPIC_IRQ_LN_OFFS))

static inline int ipic_irq_setup(int irq_vec, int line, int mode, int flags) {
    write_csr(IPIC_IDX, irq_vec);
    write_csr(IPIC_ICSR, MK_IRQ_CFG(line, mode, flags | IPIC_IRQ_CLEAR_PENDING));

    return irq_vec;
}

static inline void ipic_irq_reset(int irq_vec) {
    ipic_irq_setup(irq_vec, IPIC_IRQ_LN_VOID, IPIC_IRQ_PRIV_MMODE, IPIC_IRQ_CLEAR_PENDING);
}

static int syntacore_ipic_init(void) {
    int i;

    for (i = 0; i < PLF_IPIC_IRQ_VEC_NUM; ++i) {
        ipic_irq_reset(i);
    }

	enable_external_interrupts();

	return 0;
}

void irqctrl_enable(unsigned int interrupt_nr) {
	unsigned long state ;

    write_csr(IPIC_IDX, interrupt_nr);

    state = read_csr(IPIC_ICSR) & ~IPIC_IRQ_PENDING;
    write_csr(IPIC_ICSR, state  | IPIC_IRQ_ENABLE);
}

void irqctrl_disable(unsigned int interrupt_nr) {
	unsigned long state;

    write_csr(IPIC_IDX, interrupt_nr);
	state = read_csr(IPIC_ICSR) & ~(IPIC_IRQ_ENABLE | IPIC_IRQ_PENDING);
    
    write_csr(IPIC_ICSR, state);
}

void irqctrl_eoi(unsigned int irq) {
    write_csr(IPIC_EOI, 0);
}

unsigned int irqctrl_get_intid(void) {
    write_csr(IPIC_SOI, 0);

    return read_csr(IPIC_CISV);
}

IRQCTRL_DEF(syntacore_ipic, syntacore_ipic_init);
