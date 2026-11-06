/**
 * @file
 * @brief RISC-V Core Local Interrupt Controller (CLIC)
 *
 * @author Aleksey Zhmulin
 * @date 19.08.25
 */

#ifndef DRIVERS_INTERRUPT_RISCV_CLIC_H_
#define DRIVERS_INTERRUPT_RISCV_CLIC_H_

#include <framework/mod/options.h>
#include <module/embox/driver/interrupt/riscv_clic.h>

#define __IRQCTRL_IRQS_TOTAL \
	OPTION_MODULE_GET(embox__driver__interrupt__riscv_clic, NUMBER, irq_count)

#endif /* DRIVERS_INTERRUPT_RISCV_CLIC_H_ */
