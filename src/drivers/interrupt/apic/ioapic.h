/**
 * @file
 *
 * @date 05.02.2013
 * @author Anton Bulychev
 */

#ifndef IRQCTRL_IOAPIC_H_
#define IRQCTRL_IOAPIC_H_

#include <framework/mod/options.h>

#if OPTION_MODULE_GET(embox__driver__interrupt__ioapic, NUMBER, msi_support)
#define __IRQCTRL_IRQS_TOTAL 256
#else
#define __IRQCTRL_IRQS_TOTAL 16
#endif

#endif /* IRQCTRL_IOAPIC_H_ */
