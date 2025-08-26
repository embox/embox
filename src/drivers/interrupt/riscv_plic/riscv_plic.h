/**
 * @file
 *
 * @date 04.10.2019
 * @author Anastasia Nizharadze
 */

#ifndef DRIVERS_INTERRUPT_RISCV_PLIC_H_
#define DRIVERS_INTERRUPT_RISCV_PLIC_H_

#ifdef __EMBOX_PLATFORM__vostok_mega_015__
#define PLIC_IRQS_TOTAL 32
#else
#define PLIC_IRQS_TOTAL 1024
#endif

#define CLINT_IRQS_TOTAL 16

#define __IRQCTRL_IRQS_TOTAL (PLIC_IRQS_TOTAL + CLINT_IRQS_TOTAL)

#endif /* DRIVERS_INTERRUPT_RISCV_PLIC_H_ */
