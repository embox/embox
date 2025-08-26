/**
 * @file
 *
 * @date 01.07.24
 * @author Anton Bondarev
 */

#ifndef IRQCTRL_MIKRON_EPIC_H_
#define IRQCTRL_MIKRON_EPIC_H_

#define EPIC_IRQS_TOTAL 32

#define CLINT_IRQS_TOTAL 16

#define __IRQCTRL_IRQS_TOTAL (EPIC_IRQS_TOTAL + CLINT_IRQS_TOTAL)

#endif /* IRQCTRL_MIKRON_EPIC_H_ */
