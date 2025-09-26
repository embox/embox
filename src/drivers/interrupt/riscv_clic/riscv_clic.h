/**
 * @file
 * @brief RISC-V Core Local Interrupt Controller (CLIC)
 *
 * @author Aleksey Zhmulin
 * @date 19.08.25
 */

#ifndef DRIVERS_INTERRUPT_RISCV_CLIC_H_
#define DRIVERS_INTERRUPT_RISCV_CLIC_H_

#if defined(__EMBOX_PLATFORM__mdr1206fi__)
#define __IRQCTRL_IRQS_TOTAL 47
#elif defined(__EMBOX_PLATFORM__baikal_evu_ba__)
#define __IRQCTRL_IRQS_TOTAL 75
#endif

#endif /* DRIVERS_INTERRUPT_RISCV_CLIC_H_ */
