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

/* TODO It's an architectural part, move to src/arch/arm. */
/* SCB_BASE is used in stm32 cube, so define _SCB_BASE instead. */
#define _SCB_BASE              0xe000ed00
#define SCB_ICSR              (_SCB_BASE + 0x04)
# define SCB_ICSR_RETTOBASE   (1 << 11)
# define SCB_ICSR_PENDSVSET   (1 << 28)
#define SCB_VTOR              (_SCB_BASE + 0x08)
# define SCB_VTOR_IN_RAM      (1 << 29)

#endif /* IRQCTRL_NVIC_IMPL_H_ */
