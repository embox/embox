/**
 * @file
 *
 * @date 04.10.2019
 * @author Anastasia Nizharadze
 */

#ifndef IRQCTRL_RISCV_INTC_IMPL_H_
#define IRQCTRL_RISCV_INTC_IMPL_H_

#include <framework/mod/options.h>

#define __IRQCTRL_IRQS_TOTAL \
	OPTION_MODULE_GET(embox__driver__interrupt__riscv_plic, NUMBER, max_irq_number)

#endif /* IRQCTRL_RISCV_INTC_IMPL_H_ */
