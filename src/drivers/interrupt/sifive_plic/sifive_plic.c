/**
 * @file
 *
 * @brief PLIC interrupt driver for SiFive U54-MC Core
 *
 * @date 08.06.2024
 * @author Suraj Sonawane
 */
 
#include <assert.h>
#include <stdint.h>

#include <asm/interrupts.h>
#include <asm/regs.h>
#include <drivers/irqctrl.h>
#include <embox/unit.h>
#include <hal/reg.h>
#include <util/log.h>

#define SIFIVE_PLIC_BASE_ADDR            0x0C000000UL /* Base address for SiFive PLIC */
#define SIFIVE_PLIC_PRIORITY_OFFSET      0x00000000UL
#define SIFIVE_PLIC_PENDING_OFFSET       0x00001000UL
#define SIFIVE_PLIC_ENABLE_OFFSET        0x00002000UL
#define SIFIVE_PLIC_THRESHOLD_OFFSET     0x00200000UL
#define SIFIVE_PLIC_CLAIM_OFFSET         0x00200004UL
 
#define SIFIVE_PLIC_PRIORITY_REG(interrupt_id) (SIFIVE_PLIC_BASE_ADDR + SIFIVE_PLIC_PRIORITY_OFFSET + (interrupt_id) * 4)
#define SIFIVE_PLIC_PENDING_REG(word_index)    (SIFIVE_PLIC_BASE_ADDR + SIFIVE_PLIC_PENDING_OFFSET + (word_index) * 4)
#define SIFIVE_PLIC_ENABLE_REG(hart_id, word_index) (SIFIVE_PLIC_BASE_ADDR + SIFIVE_PLIC_ENABLE_OFFSET + (hart_id) * 0x80 + (word_index) * 4)
#define SIFIVE_PLIC_THRESHOLD_REG(hart_id)     (SIFIVE_PLIC_BASE_ADDR + SIFIVE_PLIC_THRESHOLD_OFFSET + (hart_id) * 0x1000)
#define SIFIVE_PLIC_CLAIM_REG(hart_id)         (SIFIVE_PLIC_BASE_ADDR + SIFIVE_PLIC_CLAIM_OFFSET + (hart_id) * 0x1000)
 
#define SIFIVE_PLIC_MAX_INTERRUPTS 511
#define SIFIVE_PLIC_MAX_PRIORITY 7

static int sifive_plic_init(void) {
    for (uint32_t hart_id = 0; hart_id < 4; hart_id++) {
        REG32_STORE(SIFIVE_PLIC_THRESHOLD_REG(hart_id), 0);
    }
    enable_external_interrupts();
    return 0;
}

void irqctrl_set_prio(unsigned int interrupt_nr, unsigned int prio) {
    if (interrupt_nr > 0 && interrupt_nr <= SIFIVE_PLIC_MAX_INTERRUPTS) {
        if (prio <= SIFIVE_PLIC_MAX_PRIORITY) {
            REG32_STORE(SIFIVE_PLIC_PRIORITY_REG(interrupt_nr), prio);
        } else {
            log_error("Priority value %u exceeds the maximum allowable priority %u",
                      prio, SIFIVE_PLIC_MAX_PRIORITY);
        }
    } else {
        log_error("Interrupt number %u is out of valid range (1 to %u)",
                  interrupt_nr, SIFIVE_PLIC_MAX_INTERRUPTS);
    }
}

void irqctrl_enable_in_cpu(uint32_t hart_id, unsigned int interrupt_nr) {
    if (interrupt_nr > 0 && interrupt_nr <= SIFIVE_PLIC_MAX_INTERRUPTS) {
        uint32_t reg;
        uint32_t word_index = interrupt_nr / 32;
        uint32_t bit_index = interrupt_nr % 32;

        REG32_STORE(SIFIVE_PLIC_PRIORITY_REG(interrupt_nr), 1);

        reg = REG32_LOAD(SIFIVE_PLIC_ENABLE_REG(hart_id, word_index));
        reg |= (1U << bit_index);
        REG32_STORE(SIFIVE_PLIC_ENABLE_REG(hart_id, word_index), reg);
    } else {
        log_error("Interrupt number %u is out of valid range (1 to %u)",
                  interrupt_nr, SIFIVE_PLIC_MAX_INTERRUPTS);
    }
}

void irqctrl_disable_in_cpu(uint32_t hart_id, unsigned int interrupt_nr) {
    if (interrupt_nr > 0 && interrupt_nr <= SIFIVE_PLIC_MAX_INTERRUPTS) {
        uint32_t reg;
        uint32_t word_index = interrupt_nr / 32;
        uint32_t bit_index = interrupt_nr % 32;

        reg = REG32_LOAD(SIFIVE_PLIC_ENABLE_REG(hart_id, word_index));
        reg &= ~(1U << bit_index);
        REG32_STORE(SIFIVE_PLIC_ENABLE_REG(hart_id, word_index), reg);
    } else {
        log_error("Interrupt number %u is out of valid range (1 to %u)",
                  interrupt_nr, SIFIVE_PLIC_MAX_INTERRUPTS);
    }
}

void irqctrl_eoi_in_cpu(uint32_t hart_id, unsigned int irq) {
    REG32_STORE(SIFIVE_PLIC_CLAIM_REG(hart_id), irq);
}

unsigned int irqctrl_get_intid_from_cpu(uint32_t hart_id) {
    return REG32_LOAD(SIFIVE_PLIC_CLAIM_REG(hart_id));
}

IRQCTRL_DEF(sifive_plic, sifive_plic_init);