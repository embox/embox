/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    07.03.2013
 */

#include <stddef.h>
#include <kernel/host.h>
#include <drivers/irqctrl.h>

void irqctrl_enable(unsigned int interrupt_nr) {

}

void irqctrl_disable(unsigned int interrupt_nr) {

}

void irqctrl_clear(unsigned int interrupt_nr) {

}

void irqctrl_force(unsigned int interrupt_nr) {
	emvisor_sendirq(host_getpid(), embox_getwdownstream(), EMVISOR_IRQ + interrupt_nr, NULL, 0);
}
