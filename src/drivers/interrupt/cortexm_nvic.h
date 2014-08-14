/**
 * @file
 *
 * @author  Anton Kozlov
 * @date    02.07.2012
 */

#ifndef IRQCTRL_NVIC_IMPL_H_
#define IRQCTRL_NVIC_IMPL_H_

#include <module/embox/driver/interrupt/cortexm_nvic.h>
#define __IRQCTRL_IRQS_TOTAL OPTION_MODULE_GET(embox__driver__interrupt__cortexm_nvic,NUMBER,irq_table_size)

#endif /* IRQCTRL_NVIC_IMPL_H_ */
