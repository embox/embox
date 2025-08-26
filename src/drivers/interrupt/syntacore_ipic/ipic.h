/**
 * @file
 *
 * @date 31.01.25
 * @author Anton Bondarev
 */

#ifndef IRQCTRL_SYNTACORE_IPIC_H_
#define IRQCTRL_SYNTACORE_IPIC_H_

#define IPIC_IRQS_TOTAL 32

#define CLINT_IRQS_TOTAL 16

#define __IRQCTRL_IRQS_TOTAL (IPIC_IRQS_TOTAL + CLINT_IRQS_TOTAL)

#endif /* IRQCTRL_SYNTACORE_IPIC_H_ */
