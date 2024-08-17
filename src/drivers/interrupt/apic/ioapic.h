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

#define IOAPIC_ID                     (0x0)
#define IOAPIC_VERSION                (0x1)
#define IOAPIC_ARB                    (0x2)
#define IOAPIC_REDIR_TABLE            (0x10)

/* Delivery Mode */
#define IOAPIC_ICR_DM_FIXED           (0 << 8)
#define IOAPIC_ICR_DM_LOWPRIO         (1 << 8)
#define IOAPIC_ICR_DM_SMI             (2 << 8)
#define IOAPIC_ICR_DM_NMI             (4 << 8)
#define IOAPIC_ICR_DM_INIT            (5 << 8)
#define IOAPIC_ICR_DM_EXINT           (7 << 8)

#define IOAPIC_ICR_LOGICAL_DEST       (1 << 11)
#define IOAPIC_ICR_HIGH_POLARITY      (0 << 13)
#define IOAPIC_ICR_LOW_POLARITY       (1 << 13)
#define IOAPIC_ICR_EDGE_TRIGGER       (0 << 15)
#define IOAPIC_ICR_LEVEL_TRIGGER      (1 << 15)
#define IOAPIC_ICR_INT_MASK           (1 << 16)

#endif /* IRQCTRL_IOAPIC_H_ */
