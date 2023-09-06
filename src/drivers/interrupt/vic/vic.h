/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 05.09.23
 */
#ifndef SRC_DRIVERS_INTERRUPT_VIC_H_
#define SRC_DRIVERS_INTERRUPT_VIC_H_

#include <framework/mod/options.h>
#include <config/embox/driver/interrupt/vic.h>

#define VIC_IRQ_COUNT 32

#define VIC_DEVICE_COUNT \
	OPTION_MODULE_GET(embox__driver__interrupt__vic, NUMBER, device_count)

#define __IRQCTRL_IRQS_TOTAL (VIC_DEVICE_COUNT * VIC_IRQ_COUNT)

#endif /* SRC_DRIVERS_INTERRUPT_VIC_H_ */
